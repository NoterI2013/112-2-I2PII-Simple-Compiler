#ifndef __LEX__
#define __LEX__

#define MAXLEN 256

/**
 * All possible token
 * @enum
 */
typedef enum token_set_t {
    UNKNOWN,
    END,            // \n
    ENDFILE,        // EOF
    ASSIGN,         // =
    ADDSUB_ASSIGN,  // += -=
    OR,             // |
    XOR,            // ^
    AND,            // &
    ADDSUB,         // + -
    MULDIV,         // * /
    INCDEC,         // ++ --
    INT,            // integer
    ID,             // variable
    LPAREN,         // (
    RPAREN          // )
} TokenSet;

/**
 * Test if a token matches the current token
 * @param token
 * @returns `(1|0)` as `true|false`
 */
int match(TokenSet token);

/**
 * Get the next token stored in `lexeme`
 */
void advance(void);

/**
 * Get the lexeme of the current token
 * @returns `lexeme` string (representing a token detail)
 */
char* getLexeme(void);

#endif // __LEX__