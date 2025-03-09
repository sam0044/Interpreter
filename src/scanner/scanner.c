#include "scanner.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../hash/hashtable.h"

Scanner scanner;
bool hadError = false;
Table keywordsTable;

void initScanner(const char* source){
    scanner.start = source;
    scanner.current=source;
    scanner.line=1; 
}

void initKeywordsTable(){
    initTable(&keywordsTable);
    makeEntry(&keywordsTable,"and",(void*)(uintptr_t)TOKEN_AND);
    makeEntry(&keywordsTable,"class",(void*)(uintptr_t)TOKEN_CLASS);
    makeEntry(&keywordsTable,"else",(void*)(uintptr_t)TOKEN_ELSE);
    makeEntry(&keywordsTable,"false",(void*)(uintptr_t)TOKEN_FALSE);
    makeEntry(&keywordsTable,"for",(void*)(uintptr_t)TOKEN_FOR);
    makeEntry(&keywordsTable,"fun",(void*)(uintptr_t)TOKEN_FUN);
    makeEntry(&keywordsTable,"if",(void*)(uintptr_t)TOKEN_IF);
    makeEntry(&keywordsTable,"nil",(void*)(uintptr_t)TOKEN_NIL);
    makeEntry(&keywordsTable,"or",(void*)(uintptr_t)TOKEN_OR);
    makeEntry(&keywordsTable,"print",(void*)(uintptr_t)TOKEN_PRINT);
    makeEntry(&keywordsTable,"return",(void*)(uintptr_t)TOKEN_RETURN);
    makeEntry(&keywordsTable,"super",(void*)(uintptr_t)TOKEN_SUPER);
    makeEntry(&keywordsTable,"this",(void*)(uintptr_t)TOKEN_THIS);
    makeEntry(&keywordsTable,"true",(void*)(uintptr_t)TOKEN_TRUE);
    makeEntry(&keywordsTable,"var",(void*)(uintptr_t)TOKEN_VAR);
    makeEntry(&keywordsTable,"while",(void*)(uintptr_t)TOKEN_WHILE);

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

Token makeToken(TokenType type, bool trimQuotes){
    Token token;
    token.type=type;
    const char* start = scanner.start;
    const char* current = scanner.current;
    if(trimQuotes && type==TOKEN_STRING){
        start++;
        current--;
    }
    token.length=current-start;
    token.lexeme = (char*)malloc(sizeof(char)*token.length+1);
    memcpy(token.lexeme,start,token.length);
    ((char*)token.lexeme)[token.length]='\0';
    token.line=scanner.line;
    return token;
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
                addToken(&list,makeToken(TOKEN_LEFT_PAREN, false));
                break;
            case ')': 
                addToken(&list,makeToken(TOKEN_RIGHT_PAREN, false)); 
                break;
            case '{': 
                addToken(&list,makeToken(TOKEN_LEFT_BRACE, false)); 
                break;
            case '}': 
                addToken(&list,makeToken(TOKEN_RIGHT_BRACE, false)); 
                break;
            case ',': 
                addToken(&list,makeToken(TOKEN_COMMA, false)); 
                break;
            case '.': 
                addToken(&list,makeToken(TOKEN_DOT, false)); 
                break;
            case '-': 
                addToken(&list,makeToken(TOKEN_MINUS, false)); 
                break;
            case '+': 
                addToken(&list,makeToken(TOKEN_PLUS, false)); 
                break;
            case ';': 
                addToken(&list,makeToken(TOKEN_SEMICOLON, false)); 
                break;
            case '*': 
                addToken(&list,makeToken(TOKEN_STAR, false)); 
                break;
            case '!':
                match('=') ? addToken(&list,makeToken(TOKEN_BANG_EQUAL, false)): addToken(&list,makeToken(TOKEN_BANG, false));
                break;
            case '>':
                match('=') ? addToken(&list,makeToken(TOKEN_GREATER_EQUAL, false)): addToken(&list,makeToken(TOKEN_GREATER, false));
                break;
            case '<':
                match('=') ? addToken(&list,makeToken(TOKEN_LESS_EQUAL, false)): addToken(&list,makeToken(TOKEN_LESS, false));
                break;
            case '=':
                match('=') ? addToken(&list,makeToken(TOKEN_EQUAL_EQUAL, false)): addToken(&list,makeToken(TOKEN_EQUAL, false));
                break;
            case '/':
                if(match('/')){
                    while(peek()!='\n' && peek()!='\0') advance();
                }
                else{
                    addToken(&list,makeToken(TOKEN_SLASH, false));
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
    addToken(&list, makeToken(TOKEN_EOF, false));
    freeTable(&keywordsTable);
    return list;
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
        addToken(list,makeToken(TOKEN_STRING, true));
    }
}

static void number(TokenList* list){
    while(isDigit(peek()))advance();
    if(peek()=='.' && isDigit(peekNext())){
        advance();
        while(isDigit(peek()))advance();
    }
    addToken(list,makeToken(TOKEN_NUMBER, false));
}

static void identifier(TokenList* list){
    while(isAlphaNumeric(peek()))advance();
    size_t keyLength = scanner.current-scanner.start;
    char* text = malloc(keyLength + 1);
    if (!text) {
        fprintf(stderr, "Failed to allocate memory for lexeme\n");
        return;
    }
    memcpy(text, scanner.start, keyLength);
    text[keyLength] = '\0';
    void* tokenTypePtr = getEntry(&keywordsTable,text);
    TokenType type = tokenTypePtr != NULL ? (TokenType)(uintptr_t)tokenTypePtr : TOKEN_IDENTIFIER;
    addToken(list,makeToken(type,false));
    free(text);
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