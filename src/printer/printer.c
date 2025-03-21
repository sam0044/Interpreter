#include "printer.h"
#include <stdio.h>
#include <stdlib.h>

static void printExpr(Expr* expr){
    switch(expr->type){
        case EXPR_BINARY:
            printf("(");
            printf(" %s ", expr->expression.binary.oper.lexeme);
            printValue(expr->expression.binary.left);
            printValue(expr->expression.binary.right);
            printf(")");
            break;
        case EXPR_UNARY:
            printf("(");
            printf("%s ", expr->expression.unary.oper.lexeme);
            printValue(expr->expression.unary.right);
            printf(")");
            break;
        case EXPR_GROUPING:
            printf("(group ");
            printValue(expr->expression.grouping.expression);
            printf(")");
            break;
        case EXPR_LITERAL:
            switch(expr->expression.literal.type){
                case LITERAL_INTEGER:
                    printf("%d", expr->expression.literal.value.number.integer);
                    break;
                case LITERAL_FLOAT:
                    printf("%f", expr->expression.literal.value.number.floating);
                    break;
                case LITERAL_STRING:
                    printf("\"%s\"", expr->expression.literal.value.string);
                    break;
                case LITERAL_BOOLEAN:
                    printf("%s", (expr->expression.literal.value.boolean) ? "true" : "false");
                    break;
                case LITERAL_NIL:
                    printf("nil");
                    break;
            }
            break;
        default:
            fprintf(stderr, "Invalid expression type");
            break;
    }
}

void printValue(Expr* expr){
    if(expr){
        printExpr(expr);
    }
    else{
        fprintf(stderr, "Expression is NULL");
    }
}