#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lex.h"

#define isValidIdName(c) (isalnum(c) || c == '_')

/**
 * Get token from stdin stream, put the token string into `lexeme[]`
 * @returns token type
 */
static TokenSet getToken(void);
static TokenSet curToken = UNKNOWN;
static char lexeme[MAXLEN];

TokenSet getToken(void) {
    int i = 0;
    char c = '\0';

    // remove preceeding null charactor
    while ((c = fgetc(stdin)) == ' ' || c == '\t');
    // now `c` is a non-null char

    if (isdigit(c)) {
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        while (isdigit(c) && i < MAXLEN) {
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        }
        // now `c` is not a digit (or i == MAXLEN)
        ungetc(c, stdin);
        // here error handling sux ... i may be MAXLEN
        lexeme[i] = '\0';
        return INT;
    } else if (c == '|') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        return OR;
    } else if (c == '^') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        return XOR;
    } else if (c == '&') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        return AND;
    } else if (c == '+' || c == '-') {
        lexeme[0] = c;
        char nxt = fgetc(stdin);
        if((nxt == '+' || nxt == '-') && c == nxt){
            lexeme[1] = nxt;
            lexeme[2] = '\0';
            return INCDEC;
        }else if(nxt == '='){
            lexeme[1] = '=';
            lexeme[2] = '\0';
            return ADDSUB_ASSIGN;
        }else{
            ungetc(nxt, stdin);
            lexeme[1] = '\0';
            return ADDSUB;
        }
    } else if (c == '*' || c == '/') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        return MULDIV;
    } else if (c == '\n') {
        lexeme[0] = '\0';
        return END;
    } else if (c == '=') {
        // What's wrong with u ?
        // What tf this piece of inconsistent sh*t
        strcpy(lexeme, "=");
        return ASSIGN;
    } else if (c == '(') {
        strcpy(lexeme, "(");
        return LPAREN;
    } else if (c == ')') {
        strcpy(lexeme, ")");
        return RPAREN;
    } else if (isValidIdName(c)) {
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        while(isValidIdName(c) && i < MAXLEN){
            lexeme[i++] = c;
            c = fgetc(stdin);
        }
        ungetc(c, stdin);
        if(i == MAXLEN){
            fprintf(stderr, "Input length Exceeded\n");
            exit(0);
        }
        lexeme[i] = '\0';
        return ID;
    } else if (c == EOF) {
        return ENDFILE;
    } else {
        return UNKNOWN;
    }
}

void advance(void) {
    curToken = getToken();
}

int match(TokenSet token) {
    if (curToken == UNKNOWN)
        advance();
    return token == curToken;
}

char* getLexeme(void) {
    return lexeme;
}

#undef isValidIdName