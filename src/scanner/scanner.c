#include "scanner.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

Scanner scanner;
bool hadError = false;
void initScanner(const char* source){
    scanner.start = source;
    scanner.current=source;
    scanner.line=1; 

}

void initTokenList(TokenList* list){
    list->count=0;
    list->capacity=8;
    list->tokens = (Token*)malloc(sizeof(Token)*list->capacity);
    if(!list->tokens){
        fprintf(stderr,"Failure to allocate memory for token list");
        return;
    }
}
void addToken(TokenList* list, Token token){
    if (list->count>=list->capacity){
        list->capacity*=2;
        list->tokens = realloc(list->tokens,sizeof(Token)*list->capacity);
        if(!list->tokens){
            fprintf(stderr,"Failure to reallocate memory for token list");
            return;
        }

    }
    list->tokens[list->count++] = token;
}

void freeTokenList(TokenList* list){
    for (int i = 0; i < list->count; i++) {
        free((void*)list->tokens[i].lexeme);
    }
    free(list->tokens);
    list->tokens= NULL;
    list->count=0;
    list->capacity=0;
}

Token makeToken(TokenType type){
    Token token;
    token.type=type;
    token.length=scanner.current-scanner.start;
    token.lexeme = (char*)malloc(sizeof(char)*token.length+1);
    memcpy(token.lexeme,scanner.start,token.length);
    ((char*)token.lexeme)[token.length]='\0';
    token.line=scanner.line;
    return token;
}

TokenList scanTokens(){
    TokenList list;
    initTokenList(&list);
    while(*scanner.current!='\0'){
        scanner.start = scanner.current;
        char c = advance();
        switch(c){
            case '"':
                string(&list);
                break;
            case '(': 
                addToken(&list,makeToken(TOKEN_LEFT_PAREN));
                break;
            case ')': 
                addToken(&list,makeToken(TOKEN_RIGHT_PAREN)); 
                break;
            case '{': 
                addToken(&list,makeToken(TOKEN_LEFT_BRACE)); 
                break;
            case '}': 
                addToken(&list,makeToken(TOKEN_RIGHT_BRACE)); 
                break;
            case ',': 
                addToken(&list,makeToken(TOKEN_COMMA)); 
                break;
            case '.': 
                addToken(&list,makeToken(TOKEN_DOT)); 
                break;
            case '-': 
                addToken(&list,makeToken(TOKEN_MINUS)); 
                break;
            case '+': 
                addToken(&list,makeToken(TOKEN_PLUS)); 
                break;
            case ';': 
                addToken(&list,makeToken(TOKEN_SEMICOLON)); 
                break;
            case '*': 
                addToken(&list,makeToken(TOKEN_STAR)); 
                break;
            case '!':
                match('=') ? addToken(&list,makeToken(TOKEN_BANG_EQUAL)): addToken(&list,makeToken(TOKEN_BANG));
                break;
            case '>':
                match('=') ? addToken(&list,makeToken(TOKEN_GREATER_EQUAL)): addToken(&list,makeToken(TOKEN_GREATER));
                break;
            case '<':
                match('=') ? addToken(&list,makeToken(TOKEN_LESS_EQUAL)): addToken(&list,makeToken(TOKEN_LESS));
                break;
            case '=':
                match('=') ? addToken(&list,makeToken(TOKEN_EQUAL_EQUAL)): addToken(&list,makeToken(TOKEN_EQUAL));
                break;
            case '/':
                if(match('/')){
                    while(peek()!='\n' && peek()!='\0') advance();
                }
                else{
                    addToken(&list,makeToken(TOKEN_SLASH));
                }
            case '\r':
            case '\t':
            case ' ':
                break;
            case '\n':
                scanner.line++;
                break;
            default:
                if(isDigit(c)){
                    number(&list);
                }
                else if(isAlpha(c)){
                    identifier(&list);
                }
                else{
                    error(scanner.line,"Unexpected character.");
                    hadError=true;
                }
        }
    }
    scanner.start = scanner.current;
    addToken(&list, makeToken(TOKEN_EOF));
    return list;
}

static char peek(){
    return *scanner.current;
}

static char peekNext(){
    if (*(scanner.current + 1) == '\0') return '\0';
    return *(scanner.current+1);
}

static char advance(){
    return *scanner.current++;
}

static bool match(char expected){
    if(peek()=='\0') return false;
    if(peek()!=expected) return false;
    scanner.current++;
    return true;
}

static void error(int line, char* message){
    report(line,"",message);
}

static void report(int line, char* where, char* message){
    fprintf(stderr,"[line %d] Error: %s %s",line,where,message);
    hadError=true;
}

static void string(TokenList* list){
    while(peek()!='"' && peek()!='\0'){
        if(peek()=='\n') scanner.line++;
        advance();
    }
    if(peek()=='\0'){
        hadError=true;
        error(scanner.line,"Unterminated String");
    }
    else{
        advance();
        addToken(list,makeToken(TOKEN_STRING));
    }
}

static void number(TokenList* list){
    while(isDigit(peek()))advance();
    if(peek()=='.' && isDigit(peekNext())){
        advance();
        while(isDigit(peek()))advance();
    }
    addToken(list,makeToken(TOKEN_NUMBER));
}

static void identifier(TokenList* list){
    while(isAlphaNumeric(peek()))advance();
    addToken(list,makeToken(TOKEN_IDENTIFIER));
}

static bool isDigit(char c){
    return c >= '0' && c <= '9';
}

static bool isAlpha(char c){
    return (c>='A' && c<='Z') || (c>='a' && c<='z') || c=='_';
}
static bool isAlphaNumeric(char c){
    return isDigit(c) || isAlpha(c);
}