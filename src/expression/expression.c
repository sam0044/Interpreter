#include "expression.h"
#include <stdlib.h>
#include <stdio.h>

Expr* newBinaryExpr(Expr* left, Expr* right, Token oper){
    Expr* expr = (Expr*)malloc(sizeof(Expr));
    if(!expr){
        fprintf(stderr, "Failed to allocate memory for binary expression");
        return NULL;
    }
    expr->type = EXPR_BINARY;
    expr->expression.binary.left = left;
    expr->expression.binary.right = right;
    expr->expression.binary.oper = oper;
    return expr;
}

Expr* newGroupingExpr(Expr* expression){
    Expr* expr = (Expr*)malloc(sizeof(Expr));
    if(!expr){
        fprintf(stderr, "Failed to allocate memory for grouping expression");
        return NULL;
    }
    expr->type = EXPR_GROUPING;
    expr->expression.grouping.expression = expression;
    return expr;
}

Expr* newLiteralExpr(LiteralValue value, LiteralType type){
    Expr* expr = (Expr*)malloc(sizeof(Expr));
    if(!expr){
        fprintf(stderr, "Failed to allocate memory for literal expression");
        return NULL;
    }
    expr->type = EXPR_LITERAL;
    expr->expression.literal.type = type;
    switch(type){
        case LITERAL_INTEGER:
            expr->expression.literal.value.number.integer = value.number.integer;
            break;
        case LITERAL_FLOAT:
            expr->expression.literal.value.number.floating = value.number.floating;
            break;
        case LITERAL_BOOLEAN:
            expr->expression.literal.value.boolean = value.boolean;
            break;
        case LITERAL_STRING:
            expr->expression.literal.value.string = (char*)value.string;
            break;
        case LITERAL_NIL:
            expr->expression.literal.value.nil = (char*)value.nil;
            break;
        default:
            fprintf(stderr, "Invalid literal type");
            free(expr);
            break;
    }
    return expr;
}

Expr* newUnaryExpr(Token oper, Expr* right){
    Expr* expr = (Expr*)malloc(sizeof(Expr));
    if(!expr){
        fprintf(stderr, "Failed to allocate memory for unary expression");
        return NULL;
    }
    expr->type = EXPR_UNARY;
    expr->expression.unary.oper = oper;
    expr->expression.unary.right = right;
    return expr;
}

void freeExpr(Expr* expr){
    if(!expr){
        fprintf(stderr, "Invalid expression");
        return;
    }
    switch(expr->type){
        case EXPR_BINARY:
            freeExpr(expr->expression.binary.left);
            freeExpr(expr->expression.binary.right);
            break;
        case EXPR_UNARY:
            freeExpr(expr->expression.unary.right);
            break;
        case EXPR_GROUPING:
            freeExpr(expr->expression.grouping.expression);
            break;
        case EXPR_LITERAL:
            switch(expr->expression.literal.type){
                case LITERAL_STRING:
                    free(expr->expression.literal.value.string);
                    break;
                case LITERAL_NIL:
                    free(expr->expression.literal.value.nil);
                    break;                    
            }
    }
    free(expr);
}