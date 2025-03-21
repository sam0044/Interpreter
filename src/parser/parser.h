#ifndef PARSER_H
#define PARSER_H

#include "../scanner/scanner.h"
#include "../expression/expression.h"
extern bool hadParseError;
typedef struct{
    TokenList* tokens;
    int current;
} Parser;

void initParser(TokenList* tokens);
Expr* parse();
void freeParser(Parser* parser);









#endif


