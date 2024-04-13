#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"
#include <stdbool.h>

#define max(a, b) ((a > b)? a : b)
#define min(a, b) ((a < b)? a : b)
static int regIdc = REGISTER_DEFAULT;

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
                    if (rv == 0)
                        error(DIVZERO);
                    retval = lv / rv;
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

static int reglloc(BTNode *node){
    switch(node->data){
        case INT:
            fprintf(stdout, "MOV r%d, %d\n", regIdc++, node->val);    
        break;
        case ID:
            fprintf(stdout, "MOV r%d, [%d]\n", regIdc++, node->val);    
        break;
    }
    return regIdc-1;
}

static void arithmetic(BTNode *root){
    if(root-> left->reg == NOREG) root->left->reg  = reglloc(root->left);
    if(root->right->reg == NOREG) root->right->reg = reglloc(root->right);
    int small = min(root->left->reg, root->right->reg);
    int large = max(root->left->reg, root->right->reg);
    switch(root->lexeme[0]){
        case '+':
            fprintf(stdout, "ADD r%d r%d\n", small, large);
        break;
        case '-':
            fprintf(stdout, "SUB r%d r%d\n", root->left->reg, root->right->reg);
            if(root->right->reg == small) fprintf(stdout, "MOV r%d, r%d\n", small, large);    
        break;
        case '*':
            fprintf(stdout, "MUL r%d r%d\n", small, large);
        break;
        case '/':
            fprintf(stdout, "DIV r%d r%d\n", root->left->reg, root->right->reg);
            if(root->right->reg == small) fprintf(stdout, "MOV r%d, r%d\n", small, large);    
        break;
        case '&':
            fprintf(stdout, "AND r%d r%d\n", small, large);
        break;
        case '|':
            fprintf(stdout, "OR r%d r%d\n", small, large);
        break;
        case '^':
            fprintf(stdout, "XOR r%d r%d\n", small, large);
        break;
    }
    root->reg = small;
    root->ready = true;
    regIdc--;
}

static void assignment(BTNode *root){
    if(root->right->reg == NOREG) root->right->reg = reglloc(root->right);
    // fprintf(stdout, "MOV r%d, %d\n", regIdc++, evaluateTree(root->right));
    root->right->reg = regIdc-1;
    // if(root->left->reg == NOREG) root->left->reg = reglloc(root->left);
    fprintf(stdout, "MOV [%d], r%d\n", root->left->val, root->right->reg);
    root->reg = root->right->reg;
}

#define isleaf(root) (root->left == NULL && root->right == NULL)
void assembly_Generator(BTNode* root){
    if(isleaf(root)){
        root->ready = true;
        switch(root->data){
            case INT:
                root->val = atoi(root->lexeme);
            break;
            case ID:
                root->val = getMemPos(root->lexeme);
            break;
        }
        return;
    }

    if(root->left->ready == false){
        assembly_Generator(root->left);
    }
    if(root->right->ready == false){
        assembly_Generator(root->right);
    }
    
    if(root->lexeme[0] == '=') assignment(root);
    else arithmetic(root);
}
#undef isleaf

void printPrefix(BTNode *root) {
    if (root != NULL) {
        printf("%s ", root->lexeme);
        printPrefix(root->left);
        printPrefix(root->right);
    }
}
