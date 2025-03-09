#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "scanner/scanner.h"


static void runFile(char* path);

static void runPrompt();

static void run(char* source);

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



