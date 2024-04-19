#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"
#include <stdbool.h>

static int regIdc = REGISTER_DEFAULT;
static int memIdc = MEMORY_LIMITATION;

int evaluateTree(BTNode *root) {
    int retval = 0, lv = 0, rv = 0;

    if (root != NULL) {
        switch (root->data) {
            case ID:
                retval = getval(root->lexeme);
                break;
            case INT:
                retval = atoi(root->lexeme);
                break;
            case ASSIGN:
                rv = evaluateTree(root->right);
                retval = setval(root->left->lexeme, rv);
                break;
            case ADDSUB:
            case MULDIV:
            case AND:
            case OR:
            case XOR:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                if (strcmp(root->lexeme, "+") == 0) {
                    retval = lv + rv;
                } else if (strcmp(root->lexeme, "-") == 0) {
                    retval = lv - rv;
                } else if (strcmp(root->lexeme, "*") == 0) {
                    retval = lv * rv;
                } else if (strcmp(root->lexeme, "/") == 0) {
                    if(root->right->data == INT && rv == 0) err(DIVZERO);
                    retval = (rv==0)? lv : lv/rv;
                }else if (strcmp(root->lexeme, "&") == 0) {
                    retval = lv & rv;
                }else if (strcmp(root->lexeme, "|") == 0) {
                    retval = lv | rv;
                }else if (strcmp(root->lexeme, "^") == 0) {
                    retval = lv ^ rv;
                }
                break;
            default:
                retval = 0;
        }
    }
    return retval;
}

void initRegister(){
    regIdc = REGISTER_DEFAULT;
}

static int ralloc(BTNode *node){
    switch(node->data){
        case INT:
            fprintf(stdout, "MOV r%d %s\n", regIdc++, node->lexeme);    
        break;
        case ID:
            fprintf(stdout, "MOV r%d [%d]\n", regIdc++, getMemPos(node->lexeme));    
        break;
    }
    return regIdc-1;
}

static void regfree(void){
    regIdc--;
}

static int max(int a, int b){
    return a > b? a : b;
}

static int min(int a, int b){
    return a < b? a : b;
}

static int depth(BTNode* root){
    if(root == NULL) return 0;
    else if(root->data == ASSIGN) return depth(root->right);
    else return 1 + max(depth(root->left), depth(root->right));
}

static void assembly_Generator(BTNode* root);

static void arithmetic(BTNode *root){
    bool use_temp_reg = regIdc == 7;
    if(use_temp_reg){
        fprintf(stdout, "MOV [%d] r6\n", memIdc * 4);
        regfree();
        memIdc--;
    }

    if(depth(root->left) >= depth(root->right)){
        assembly_Generator(root->left);
        assembly_Generator(root->right);
    }else{
        assembly_Generator(root->right);
        assembly_Generator(root->left);
    }

    int small = min(root->left->reg, root->right->reg);
    int large = max(root->left->reg, root->right->reg);
    int subjectReg = use_temp_reg? large : small;
    int  objectReg = use_temp_reg? small : large;

    switch(root->lexeme[0]){
        case '+':
            fprintf(stdout, "ADD r%d r%d\n", subjectReg, objectReg);
        break;
        case '-':
            fprintf(stdout, "SUB r%d r%d\n", root->left->reg, root->right->reg);
            if(root->right->reg == subjectReg) fprintf(stdout, "MOV r%d r%d\n", subjectReg, objectReg);    
        break;
        case '*':
            fprintf(stdout, "MUL r%d r%d\n", subjectReg, objectReg);
        break;
        case '/':
            fprintf(stdout, "DIV r%d r%d\n", root->left->reg, root->right->reg);
            if(root->right->reg == subjectReg) fprintf(stdout, "MOV r%d r%d\n", subjectReg, objectReg);    
        break;
        case '&':
            fprintf(stdout, "AND r%d r%d\n", subjectReg, objectReg);
        break;
        case '|':
            fprintf(stdout, "OR r%d r%d\n", subjectReg, objectReg);
        break;
        case '^':
            fprintf(stdout, "XOR r%d r%d\n", subjectReg, objectReg);
        break;
    }
    // Up pass the subject register to current node
    root->reg = subjectReg;

    if(use_temp_reg) fprintf(stdout, "MOV r6 [%d]\n", ++memIdc * 4);
    else regfree();
}

static void assignment(BTNode *root){
    assembly_Generator(root->right);
    fprintf(stdout, "MOV [%d] r%d\n", getMemPos(root->left->lexeme), root->right->reg);
    root->reg = root->right->reg;
}

#define isleaf(root) (root->left == NULL && root->right == NULL)
static void assembly_Generator(BTNode* root){
    if (root == NULL) return;
    
    switch (root->data){
        case INT:
        case ID:
            root->reg = ralloc(root);
            break;
        case ASSIGN:
            assignment(root);
            break;
        default:
            arithmetic(root);
            break;
    }
}
#undef isleaf

void puring_assembly(BTNode* root){
    if(root == NULL) return;

    if(root->data == ASSIGN){
        assembly_Generator(root);
        regfree();
    }else{
        puring_assembly(root->left);
        puring_assembly(root->right);
    }
}

void printPrefix(BTNode *root) {
    if (root != NULL) {
        printf("%s ", root->lexeme);
        printPrefix(root->left);
        printPrefix(root->right);
    }
}