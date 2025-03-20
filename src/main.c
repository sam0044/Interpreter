#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "scanner/scanner.h"
#include "expression/expression.h"
#include "printer/printer.h"


static void runFile(char* path);

static void runPrompt();

static void run(char* source);

void testExpressionCreation() {
    // Create a literal for 123
    LiteralValue numVal123;
    numVal123.number.integer = 123;
    Expr* literal123 = newLiteralExpr(numVal123, LITERAL_INTEGER);
    if (!literal123) {
        fprintf(stderr, "Failed to create literal for 123\n");
        return;
    }
    
    // Create the unary minus token
    Token minusToken;
    minusToken.type = TOKEN_MINUS;
    minusToken.lexeme = "-";
    minusToken.length = 1;
    minusToken.line = 1;
    
    // Create the unary expression -123
    Expr* unaryMinus = newUnaryExpr(minusToken, literal123);
    if (!unaryMinus) {
        fprintf(stderr, "Failed to create unary minus\n");
        freeExpr(literal123);
        return;
    }
    
    // Create a literal for 45.67
    LiteralValue numVal4567;
    numVal4567.number.floating = 45.67;
    Expr* literal4567 = newLiteralExpr(numVal4567, LITERAL_FLOAT);
    if (!literal4567) {
        fprintf(stderr, "Failed to create literal for 45.67\n");
        freeExpr(unaryMinus);
        return;
    }
    
    // Create the grouping expression (45.67)
    Expr* grouping4567 = newGroupingExpr(literal4567);
    if (!grouping4567) {
        fprintf(stderr, "Failed to create grouping\n");
        freeExpr(unaryMinus);
        freeExpr(literal4567);
        return;
    }
    
    // Create the multiplication token
    Token starToken;
    starToken.type = TOKEN_STAR;
    starToken.lexeme = "*";
    starToken.length = 1;
    starToken.line = 1;
    
    // Create the binary expression (- 123) * (group 45.67)
    Expr* binaryMult = newBinaryExpr(unaryMinus, grouping4567, starToken);
    if (!binaryMult) {
        fprintf(stderr, "Failed to create binary mult\n");
        freeExpr(unaryMinus);
        freeExpr(grouping4567);
        return;
    }
    
    // Print the expression
    printf("Expression test: ");
    printExpr(binaryMult);
    printf("\n");
    
    // Free the entire expression tree
    freeExpr(binaryMult);
}

int main(int argc, char* argv[]){
    if(argc>2){
        fprintf(stderr,"Usage: lox [script]");
        exit(EXIT_FAILURE);
    }
    else if (argc == 2)
    {
        // gets the absolute path of the file
        char* absolute_path = realpath(argv[1],NULL);
        if(!absolute_path){
            fprintf(stderr,"Could not find the absolute path \"%s\"",absolute_path);
        }
        runFile(absolute_path);
    }
    else {
        runPrompt();
    }
    
}

// Implementation of run functions

static void runFile(char* path){
    FILE* file;
    file = fopen(path,"rb");
    if(!file){
        fprintf(stderr,"Failed to open file at \"%s\"",path);
        return;
    }
    // gets the size of the file for buffer allocation
    fseek(file,0,SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    char* buffer = (char*)malloc(file_size+1);

    if (!buffer){
        fprintf(stderr, "Failed to allocate memory \"%s\"",path);
        fclose(file);
        return;
    }
    size_t bytes_read = fread(buffer,sizeof(char),file_size,file);
    if(bytes_read!=file_size){
        fprintf(stderr,"Error reading file into the buffer");
        fclose(file);
        free(buffer);
        return;
    }
    buffer[bytes_read]='\0';
    run(buffer);
    fclose(file);
    free(buffer);
}

static void runPrompt(){
    char line[1024];
    for (;;){
        printf("> ");
        fflush(NULL);
        if(fgets(line,sizeof(line),stdin)==NULL){
            break;
        }
        run(line);
        hadError=false;
    }
}

static void run(char* source){
    initScanner(source);
    initKeywordsTable();
    TokenList list = scanTokens();
    for(int i=0;i<list.count;i++){
        Token token = list.tokens[i];
        printf("Address: %p  Type: %d Token: \"%s\"\n",(void*)&list.tokens[i],token.type,token.lexeme);
    }
    freeTokenList(&list);

}




