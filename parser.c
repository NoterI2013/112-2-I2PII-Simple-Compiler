#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codeGen.h"

/**************************************************************************
 *                               -= IDEA =-                               *
 * The main idea is to use grammer relation to build abstruct syntax tree *
 * - `lexeme[]` is always to-be-processed                                 *
 * - use `match()` to check `lexeme[]`'s type                             *
 * - use `getLexeme()` to get underling token data                        *
 * - lastly use `advance()` to hot update `lexeme[]`                      *
 *   in order to make `lexeme[]` to-be-processed                          *
 * - grammer is the following                                             *
 *   - statement                                                          *
 *     - ENDFILE                                                          *
 *     - END                                                              *
 *     - expr END                                                         *
 *   - expr                                                               *
 *     - term expr_tail                                                   *
 *   - expr_tail                                                          *
 *     - ADDSUB term expr_tail                                            *
 *     - NiL                                                              *
 *   - term                                                               *
 *     - factor term_tail                                                 *
 *   - term_tail                                                          *
 *     - MULDIV factor term_tail                                          *
 *     - NiL                                                              *
 *   - factor                                                             *
 *     - INT                                                              *
 *     - ADDSUB INT                                                       *
 *     - ADDSUB ID                                                        *
 *     - ADDSUB LPAREN expr RPAREN                                        *
 *     - ID                                                               *
 *     - ID ASSIGN expr                                                   *
 *     - LPAREN expr RPAREN                                               *
 **************************************************************************/

/**
 * symbol count
 */
int sbcount = 0;
/**
 * valiable table for lookups
 */
Symbol table[TBLSIZE];

void initTable(void) {
    strcpy(table[0].name, "x");
    table[0].val = 1;
    strcpy(table[1].name, "y");
    table[1].val = 1;
    strcpy(table[2].name, "z");
    table[2].val = 1;
    sbcount = 3;
}

int getval(char* str) {
    int i = 0;

    // default: will not exceeed
    for (i = 0; i < sbcount; i++)
        if (strcmp(str, table[i].name) == 0)
            return table[i].val;

    // put the boundary check after the iteration is fine :)
    if (sbcount >= TBLSIZE)
    error(RUNOUT);

    // strcpy(table[sbcount].name, str);
    // table[sbcount].val = 0;
    // sbcount++;
    err(NOTFOUND);
    return 0;
}

int getMemPos(char* str){
    for (int i=0; i < sbcount; i++){
        if (strcmp(str, table[i].name) == 0){
            return i*4;
        }
    }
    
    if (sbcount >= TBLSIZE)
    error(RUNOUT);

    strcpy(table[sbcount].name, str);
    table[sbcount].val = 0;
    sbcount++;
    return (sbcount-1)*4;
}

int setval(char* str, int val) {
    int i = 0;

    for (i = 0; i < sbcount; i++) {
        if (strcmp(str, table[i].name) == 0) {
            table[i].val = val;
            return val;
        }
    }

    if (sbcount >= TBLSIZE)
    error(RUNOUT);

    strcpy(table[sbcount].name, str);
    table[sbcount].val = val;
    sbcount++;
    return val;
}

BTNode* makeNode(TokenSet tok, const char* lexe) {
    BTNode* node = (BTNode*)malloc(sizeof(BTNode));
    strcpy(node->lexeme, lexe);
    node->data = tok;
    node->val = 0;
    node->reg = NOREG;
    node->ready = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void freeTree(BTNode* root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

#define isleaf(root) ((root)->left == NULL && (root)->right == NULL)
#define isZERO(root) ((root)->data == INT && evaluateTree((root)) == 0)
#define isONE(root) ((root)->data == INT && evaluateTree((root)) == 1)
#define isNegativeONE(root) ((root)->data == INT && evaluateTree((root)) == -1)
static void optimize(BTNode** root){
    if((*root) == NULL && isleaf((*root))) return;

    if( !isleaf((*root)->left) ) optimize((&(*root)->left));
    if( !isleaf((*root)->right) ) optimize((&(*root)->right));

    // Optimize: arithmetic constant-constant
    if((*root)->left->data == INT && (*root)->right->data == INT){
        char buf[MAXLEN];
        sprintf(buf, "%d", evaluateTree((*root)));
        BTNode* newNode = makeNode(INT, buf);
        freeTree(*root);
        *root = newNode;
    }
/*
    // Optimize: addition zero
    else if((*root)->data == ADDSUB && (*root)->lexeme[0] == '+'){
        BTNode* subRemain = NULL;
        if(isZERO((*root)->left)){
            subRemain = (*root)->right;
            (*root)->right = NULL;
        }else if(isZERO((*root)->right)){
            subRemain = (*root)->left;
            (*root)->left = NULL;
        }
        freeTree(*root);
        *root = subRemain;
    }

    // Optimize: subtract any-zero
    else if( (*root)->data == ADDSUB && (*root)->lexeme[0] == '-' ){
        BTNode* subRemain = NULL;
        if(isZERO((*root)->right)){
            subRemain = (*root)->left;
            (*root)->left = NULL;
        }
        freeTree(*root);
        *root = subRemain;
    }

    // Optimize: multiply zero | one | -1 
    else if( (*root)->data == MULDIV && (*root)->lexeme[0] == '*' ){
        BTNode* subRemain = NULL;
        if(isZERO((*root)->left)){
//            subRemain = (*root)->right;
//            (*root)->right = NULL;
        }else if(isZERO((*root)->right)){
//            subRemain = (*root)->left;
//            (*root)->left = NULL;
        }else if(isONE((*root)->left)){
            subRemain = (*root)->right;
            (*root)->right = NULL;
        }else if(isONE((*root)->right)){
            subRemain = (*root)->left;
            (*root)->left = NULL;
        }else if(isNegativeONE((*root)->left)){
//            subRemain = (*root)->right;
//            (*root)->right = NULL;
        }else if(isNegativeONE((*root)->right)){
//            subRemain = (*root)->left;
//            (*root)->left = NULL;
        }
        freeTree(*root);
        *root = subRemain;
    }
*/
}
#undef isleaf
#undef isZERO
#undef isONE
#undef isNegativeONE

// statement := ENDFILE | END | expr END
void statement(void) {
    BTNode* retp = NULL;

    if (match(ENDFILE)) {
        for(int i=0; i<3; i++){
            fprintf(stdout, "MOV r%d, [%d]\n", i, i*4);
        }
        fprintf(stdout, "EXIT 0\n");
        exit(0);
    } else if (match(END)) {
//        printf(">> ");
        advance();
    } else {
        retp = assign_expr();
        if (match(END)) {
            optimize(&retp);
            evaluateTree(retp);
            if(PRINTERR){
                printf("Prefix traversal: ");
                printPrefix(retp);
                printf("\n");
            }
            initRegister();
            puring_assembly(retp);
            // printf("\n");
            freeTree(retp);
            // printf(">> ");
            advance();
        } else {
            error(SYNTAXERR);
        }
    }
}

// assign_expr := ID ASSIGN assign_expr | ID ADDSUB_ASSIGN assign_expr | or_expr
BTNode* assign_expr(){
    // an or_expr can be the LHS in a assignment;
    BTNode* left = or_expr();
    BTNode* parent = NULL;
    if(match(ASSIGN) || match(ADDSUB_ASSIGN)){
        if(left->data != ID) err(NOTLVAL);
    }else{
        // if not an assignment, the parsing terminate...
        return left;
    }

    if(match(ASSIGN)){
        parent = makeNode(ASSIGN, "=");
        advance();
        parent->left = left;
        parent->right= assign_expr();
    } else if(match(ADDSUB_ASSIGN)){
        parent = makeNode(ASSIGN, "=");
        parent->left = left;
        parent->right = makeNode(ADDSUB, (char[2]){getLexeme()[0], '\0'});
        advance();
        parent->right->left = makeNode(ID, parent->left->lexeme);
        parent->right->right= assign_expr();
    }
    return  parent;
}

// or_expr := xor_expr or_expr_tail
BTNode* or_expr(void) {
    BTNode* left = xor_expr();
    return or_expr_tail(left);
}

// or_expr_tail := OR xor_expr or_expr_tail | NiL
BTNode* or_expr_tail(BTNode* left) {
    if(match(OR)){
        BTNode* localRoot = makeNode(OR, "|");
        advance();
        localRoot->left = left;
        localRoot->right= or_expr();
        return or_expr_tail(localRoot);
    }else{ // Nil, return what has been parsed directly...
        return left;
    }
}

// xor_expr := and_expr xor_expr_tail
BTNode* xor_expr(void) {
    BTNode* left = and_expr();
    return xor_expr_tail(left);
}

// xor_expr_tail := XOR and_expr xor_expr_tail | NiL
BTNode* xor_expr_tail(BTNode* left) {
    if(match(XOR)){
        BTNode* localRoot = makeNode(XOR, "^");
        advance();
        localRoot->left = left;
        localRoot->right= and_expr();
        return xor_expr_tail(localRoot);
    }else{ // Nil, return what has been parsed directly...
        return left;
    }
}

// and_expr := addsub_expr and_expr_tail
BTNode* and_expr(void) {
    BTNode* left = addsub_expr();
    return and_expr_tail(left);
}

// and_expr_tail := AND addsub_expr and_expr_tail | NiL
BTNode* and_expr_tail(BTNode* left) {
    if(match(AND)){
        BTNode* localRoot = makeNode(AND, "&");
        advance();
        localRoot->left = left;
        localRoot->right= addsub_expr();
        return and_expr_tail(localRoot);
    }else{ // Nil, return what has been parsed directly...
        return left;
    }
}

// addsub_expr := muldiv_expr addsub_expr_tail
BTNode* addsub_expr(void) {
    BTNode* left = muldiv_expr();
    return addsub_expr_tail(left);
}

// addsub_expr_tail := ADDSUB muldiv_expr addsub_expr_tail | NiL
BTNode* addsub_expr_tail(BTNode* left) {
    if(match(ADDSUB)){
        BTNode* localRoot = makeNode(ADDSUB, getLexeme());
        advance();
        localRoot->left = left;
        localRoot->right= muldiv_expr();
        return addsub_expr_tail(localRoot);
    }else{ // Nil, return what has been parsed directly...
        return left;
    }
}

// muldiv_expr := unary_expr muldiv_expr_tail
BTNode* muldiv_expr(void) {
    BTNode* left = unary_expr();
    return muldiv_expr_tail(left);
}

// muldiv_expr_tail := MULDIV unary_expr muldiv_expr_tail | NiL
BTNode* muldiv_expr_tail(BTNode* left) {
    if(match(MULDIV)){
        BTNode* localRoot = makeNode(MULDIV, getLexeme());
        advance();
        localRoot->left = left;
        localRoot->right= unary_expr();
        return muldiv_expr_tail(localRoot);
    }else{ // Nil, return what has been parsed directly...
        return left;
    }
}

// unary_expr := ADDSUB unary_expr | factor := <+->*n factor
BTNode* unary_expr(void) {
    int negative_switch = 0;
    while(match(ADDSUB)){
        negative_switch ^= getLexeme()[0] == '-';
        advance();
    }
    BTNode* right = factor();
    if(negative_switch){
        BTNode* parentRoot = makeNode(ADDSUB, "-");
        parentRoot->left = makeNode(INT, "0");
        parentRoot->right= right;
        return parentRoot;
    }else{
        return right;
    }

}

// factor := INT | ID | INCDEC ID | LPAREN assign_expr RPAREN
BTNode* factor(void) {
    BTNode *localRoot = NULL;
    if (match(INT)) {
        localRoot = makeNode(INT, getLexeme());
        advance();
    } else if (match(ID)) {
        localRoot = makeNode(ID, getLexeme());
        advance();
    } else if (match(INCDEC)){
        char addsub_operation = getLexeme()[0];
        advance();
        BTNode* next = factor();
        if (next->data == ID) {
            localRoot = makeNode(ASSIGN, "=");
            localRoot->left = next;
            localRoot->right = makeNode(ADDSUB, (char[2]){ addsub_operation, '\0' });
            localRoot->right->left = makeNode(ID, next->lexeme);
            localRoot->right->right = makeNode(INT, "1");
        } else {
            err(SYNTAXERR);
        }
    } else if (match(LPAREN)){
        advance();
        localRoot = assign_expr();
        if (match(RPAREN))
            advance();
        else
            err(SYNTAXERR);
    } else {
        err(NOTNUMID);
    }
    return localRoot;
}

void err(ErrorType errorNum) {
    if (PRINTERR) {
        fprintf(stderr, "error: ");
        switch (errorNum) {
            case MISPAREN:
                fprintf(stderr, "mismatched parenthesis\n");
                break;
            case NOTNUMID:
                fprintf(stderr, "number or identifier expected\n");
                break;
            case NOTFOUND:
                fprintf(stderr, "variable not defined\n");
                break;
            case RUNOUT:
                fprintf(stderr, "out of memory\n");
                break;
            case NOTLVAL:
                fprintf(stderr, "lvalue required as an operand\n");
                break;
            case DIVZERO:
                fprintf(stderr, "divide by constant zero\n");
                break;
            case SYNTAXERR:
                fprintf(stderr, "syntax error\n");
                break;
            default:
                fprintf(stderr, "undefined error\n");
                break;
        }
    }
    fprintf(stdout, "EXIT %d", 1);
    exit(0);
}