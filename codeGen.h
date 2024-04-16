#ifndef __CODEGEN__
#define __CODEGEN__

#include "parser.h"

#define REGISTER_DEFAULT 0

// Evaluate the syntax tree
int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
void printPrefix(BTNode *root);

void initRegister();
// void assembly_Generator(BTNode* root);
void puring_assembly(BTNode* root);

#endif // __CODEGEN__
