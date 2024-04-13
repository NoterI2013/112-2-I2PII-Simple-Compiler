#ifndef __CODEGEN__
#define __CODEGEN__

#include "parser.h"

#define REGISTER_DEFAULT 0

// Evaluate the syntax tree
extern int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
extern void printPrefix(BTNode *root);

extern void initRegister();
extern void assembly_Generator(BTNode* root);

#endif // __CODEGEN__
