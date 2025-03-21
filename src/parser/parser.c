#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool hadParseError = false;
Parser parser;
void initParser(TokenList* tokens){
    parser.tokens = tokens;
    parser.current = 0;
}

static Token previous();
static Token peek();
static bool isAtEnd();
static Token advance();
static bool match(TokenType type);
static Token consume(TokenType type, char* message);
static void report(int line, char* where, const char* lexeme, char* message);
static void error(Token token, char* message);
static void synchronize();
static Expr* expression();
static Expr* equality();
static Expr* comparison();
static Expr* term();
static Expr* factor();
static Expr* unary();
static Expr* primary();


static Expr* expression(){
    return equality();
}

Expr* parse(){
    return expression();
}

static Expr* equality(){
    Expr* expr = comparison();
    while(match(TOKEN_BANG_EQUAL) || match(TOKEN_EQUAL_EQUAL)){
        Token operator = previous();
        Expr* right = comparison();
        expr = newBinaryExpr(expr, operator, right);
    }
    return expr;
}

static Expr* comparison(){
    Expr* expr = term();
    while(match(TOKEN_GREATER) || match(TOKEN_GREATER_EQUAL) || match(TOKEN_LESS) || match(TOKEN_LESS_EQUAL)){
        Token operator = previous();
        Expr* right = term();
        expr = newBinaryExpr(expr, operator, right);
    }
    return expr;
}

static Expr* term(){
    Expr* expr = factor();
    while(match(TOKEN_MINUS) || match(TOKEN_PLUS)){
        Token operator = previous();
        Expr* right = factor();
        expr = newBinaryExpr(expr, operator, right);
    }
    return expr;
}

static Expr* factor(){
    Expr* expr = unary();
    while(match(TOKEN_SLASH) || match(TOKEN_STAR)){
        Token operator = previous();
        Expr* right = unary();
        expr = newBinaryExpr(expr, operator, right);
    }
    return expr;
}

static Expr* unary(){
    while(match(TOKEN_BANG) || match(TOKEN_MINUS)){
        Token operator = previous();
        Expr* right = unary();
        return newUnaryExpr(operator, right);
    }
    return primary();
}

static Expr* primary(){
    if(match(TOKEN_FALSE)){
        LiteralValue value;
        value.boolean = false;
        return newLiteralExpr(value, LITERAL_BOOLEAN);
    } 
    if(match(TOKEN_TRUE)){
        LiteralValue value;
        value.boolean = true;
        return newLiteralExpr(value, LITERAL_BOOLEAN);
    }
    if(match(TOKEN_NIL)){
        LiteralValue value;
        value.nil = NULL;
        return newLiteralExpr(value, LITERAL_NIL);  
    }
    if(match(TOKEN_NUMBER)){
        Token token = previous();
        LiteralValue value;
        if(strchr(token.lexeme, '.')){
            value.number.floating = atof(token.lexeme);
            return newLiteralExpr(value, LITERAL_FLOAT);
        }
        value.number.integer = atoi(token.lexeme);
        return newLiteralExpr(value, LITERAL_INTEGER);
    }
    if(match(TOKEN_STRING)){
        Token token = previous();
        LiteralValue value;
        value.string = strdup(token.lexeme);
        if (!value.string) {
            fprintf(stderr, "Failed to allocate memory for string literal\n");
            return NULL;
        }
        return newLiteralExpr(value, LITERAL_STRING);
    }
    if(match(TOKEN_LEFT_PAREN)){
        Expr* expr = expression();
        consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
        return newGroupingExpr(expr);
    }

    error(peek(), "Expect expression.");
    return NULL; 
    
}

static Token previous(){
    return parser.tokens->tokens[parser.current-1];
}

static Token peek(){
    return parser.tokens->tokens[parser.current];
}

static bool isAtEnd(){
    return peek().type == TOKEN_EOF;
}

static Token advance(){
    if(!isAtEnd()) parser.current+=1;
    return previous();
}

static bool match(TokenType type){
    if(isAtEnd()) return false;
    if(peek().type==type){
        advance();
        return true;
    }
    return false;

}

static Token consume(TokenType type, char* message){
    if(!isAtEnd() && peek().type==type){
        return advance();
    }
    error(peek(), message);
    return peek();
}

static void report(int line, char* where, const char* lexeme, char* message){
    if (lexeme) {
        fprintf(stderr, "[line %d] Error%s '%s': %s\n", line, where, lexeme, message);
    } else {
        fprintf(stderr, "[line %d] Error%s: %s\n", line, where, message);
    }
}

static void error(Token token, char* message){
    if(token.type==TOKEN_EOF) report(token.line, " at end",NULL, message);
    else report(token.line, " at", token.lexeme, message);
    hadParseError = true;
}

static void synchronize(){
    advance();
    while(!isAtEnd()){
        if(previous().type==TOKEN_SEMICOLON) return;
        switch(peek().type){
            case TOKEN_CLASS:
            case TOKEN_FUN:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_PRINT:
            case TOKEN_RETURN:
                return;
        }
        advance();
    }
}