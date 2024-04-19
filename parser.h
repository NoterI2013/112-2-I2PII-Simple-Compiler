#ifndef __PARSER__
#define __PARSER__

#include "lex.h"
#define TBLSIZE 64
#define NOREG -1

/**
 * Set PRINTERR to 1 to print error message while calling error()
 * @todo set PRINTERR to 0 before you submit your code
 */
#define PRINTERR 0

/**
 * Macro to print error message and exit the program
 * This will also print where you called it in your program
 */
#define error(errorNum) { \
    if (PRINTERR) \
        fprintf(stderr, "error() called at %s:%d: ", __FILE__, __LINE__); \
    err(errorNum); \
}

/**
 * Error types
 * @enum
 */
typedef enum error_type_t {
    UNDEFINED, // variable not defined
    MISPAREN,  // parentheses not matched
    NOTNUMID,  //
    NOTFOUND,  //
    RUNOUT,    // variable table out-of-bound
    NOTLVAL,   //
    DIVZERO,   //
    SYNTAXERR  //
} ErrorType;

/**
 * Structure of the symbol table
 * @struct
 */
typedef struct {
    int val;
    char name[MAXLEN];
} Symbol;

/**
 * Structure of a tree node
 * @struct
 */
typedef struct _Node {
    TokenSet data;
    int val;
    int reg;
    int ready;
    char lexeme[MAXLEN];
    struct _Node *left;
    struct _Node *right;
} BTNode;

/**
 * The symbol table
 */
extern Symbol table[TBLSIZE];

/**
 * There would be `x/y/z` symbol initially, value is `0`
 */
void initTable(void);

/**
 * Get the value of variable stored in table
 * If no exists then create a new var and return 0
 * @param str varuable name
 * @returns value of variable
 * @warning Will blame if exceed table capacity
 */
int getval(char* str);

/**
 * Get the memory position of the an variable
 * 
 * @param str variable name
 * @return position of the memory
 */
int getMemPos(char* str);

/**
 * Set the value of variable stored in table
 * If no exists then create a new var, set the value, and return `val`
 * @param str variable name
 * @param val to-bo-set value
 * @returns value of variable
 * @warning Will blame if exceed table capacity
 */
int setval(char* str, int val);

/**
 * Make a new node according to token type and lexeme
 * @param tok
 * @param lexe
 * @returns ast node
 */
BTNode* makeNode(TokenSet tok, const char* lexe);

/**
 * Free the syntax tree
 */
void freeTree(BTNode *root);

void statement(void);
BTNode* assign_expr(void);
BTNode* or_expr(void);
BTNode* or_expr_tail(BTNode* left);
BTNode* xor_expr(void);
BTNode* xor_expr_tail(BTNode* left);
BTNode* and_expr(void);
BTNode* and_expr_tail(BTNode* left);
BTNode* addsub_expr(void);
BTNode* addsub_expr_tail(BTNode* left);
BTNode* muldiv_expr(void);
BTNode* muldiv_expr_tail(BTNode* left);
BTNode* unary_expr(void);
BTNode* factor(void);


// Print error message and exit the program
void err(ErrorType errorNum);

#endif // __PARSER__