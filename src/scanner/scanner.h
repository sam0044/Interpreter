#ifndef SCANNER_H
#define SCANNER_H
#include <stddef.h>
#include <stdbool.h>

extern bool hadError;
typedef enum TokenType{
    // Single-character tokens.
TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,
// One or two character tokens.
TOKEN_BANG, TOKEN_BANG_EQUAL,
TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
TOKEN_GREATER, TOKEN_GREATER_EQUAL,
TOKEN_LESS, TOKEN_LESS_EQUAL,
// Literals.
TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,
// Keywords.
TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE,
TOKEN_EOF

} TokenType;

typedef struct{
    TokenType type;
    const char* lexeme;
    size_t length;
    int line;

} Token;

typedef struct{
    Token* tokens;
    size_t count;
    size_t capacity;
} TokenList;

typedef struct{
    const char* start;
    const char* current;
    int line;

} Scanner;

// token list for storing tokens
void initTokenList(TokenList* list);
Token makeToken(TokenType type, bool trimQuotes);
void addToken(TokenList* list, Token token);
void freeTokenList(TokenList* list);

// scanner functions
void initScanner(const char* source);
void initKeywordsTable();
TokenList scanTokens();


#endif