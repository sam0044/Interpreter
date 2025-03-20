#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "../scanner/scanner.h"

typedef struct Expr Expr;
typedef struct BinaryExpr BinaryExpr;
typedef struct GroupingExpr GroupingExpr;
typedef struct LiteralExpr LiteralExpr;
typedef struct UnaryExpr UnaryExpr;

typedef enum ExprType{
    EXPR_BINARY,
    EXPR_GROUPING,
    EXPR_LITERAL,
    EXPR_UNARY
} ExprType;


typedef struct BinaryExpr{
    Expr* left;
    Expr* right;
    Token oper;
} BinaryExpr;

typedef struct GroupingExpr{
    Expr* expression;
} GroupingExpr;

typedef enum LiteralType{
    LITERAL_INTEGER,
    LITERAL_FLOAT,
    LITERAL_STRING,
    LITERAL_BOOLEAN,
    LITERAL_NIL
} LiteralType;

typedef union Number{
    double floating;
    int integer;
} Number;

typedef union LiteralValue{
    Number number;
    char* string;
    bool boolean;
    char* nil;
} LiteralValue;

typedef struct LiteralExpr{
    LiteralType type;
    LiteralValue value;
} LiteralExpr;

typedef struct UnaryExpr{
    Token oper;
    Expr* right;
} UnaryExpr;

typedef struct Expr{
    ExprType type;
    union {
        BinaryExpr binary;
        GroupingExpr grouping;
        LiteralExpr literal;
        UnaryExpr unary;
    } expression;
} Expr;

Expr* newBinaryExpr(Expr* left, Expr* right, Token oper);
Expr* newGroupingExpr(Expr* expression);
Expr* newLiteralExpr(LiteralValue value, LiteralType type);
Expr* newUnaryExpr(Token oper, Expr* right);
void freeExpr(Expr* expr);

#endif
