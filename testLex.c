// This is a test code for developement, not included in CMakeLists.txt
#include <stdio.h>
#include "lex.h"

int main() {
    while (1) {
        advance();
        printf(": %s\n", getLexeme());
    }

    return 0;
}