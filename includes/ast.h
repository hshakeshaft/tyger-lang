#ifndef TYGER_AST_H_
#define TYGER_AST_H_
#include <stdint.h>

#include "lexer.h"

#define AST_STATEMENT_KIND_LIST \
    X(ILLGEAL_STATEMENT)        \
    X(VAR_STATEMENT)            \
    X(RETURN_STATEMENT)         \
    X(EXPRESSION_STATEMENT)

#define AST_EXPRESSION_KIND_LIST \
    X(IDENT_EXPRESSION)          \
    X(INT_EXPRESSION)            \
    X(FLOAT_EXPRESSION)          \
    X(PREFIX_EXPRESSION)         \
    X(INFIX_EXPRESSION)

typedef enum
{
    #define X(NAME) AST_##NAME,
    AST_STATEMENT_KIND_LIST
    #undef X
} Statement_Kind;

// NOTE(HS): need to forward declare to allow nesting of expressions
typedef struct expression_s Expression;

typedef enum
{
    #define X(NAME) AST_##NAME,
    AST_EXPRESSION_KIND_LIST
    #undef X
} Expression_Kind;

typedef struct
{
    const char *ident;
} Ident_Expression;

typedef struct
{
    int32_t value;
} Int_Expression;

typedef struct
{
    float value;
} Float_Expression;

typedef struct
{
    char op;
    Expression *rhs;
} Prefix_Expression;

typedef struct
{
    char op[2];
    Expression *lhs;
    Expression *rhs;
} Infix_Expression;

typedef union
{
    Ident_Expression ident_expression;
    Int_Expression int_expression;
    Float_Expression float_expression;
    Prefix_Expression prefix_expression;
    Infix_Expression infix_expression;
} uExpression;

struct expression_s
{
    Expression_Kind kind;
    uExpression expr;
};


typedef struct
{
    Token token;
} Illegal_statement;

typedef struct
{
    const char *ident;
    // TODO(HS): add expression
} Var_Statement;

typedef struct
{
    Token token;
    // TODO(HS): add expression
} Return_Statement;

typedef struct
{
    Expression expression;
} Expression_Statement;

typedef union
{
    Illegal_statement illegal_statement;
    Var_Statement var_statement;
    Return_Statement return_statement;
    Expression_Statement expression_statement;
} uStatement;

typedef struct
{
    Statement_Kind kind;
    uStatement stmt;
} Statement;


#if defined(__cplusplus)
extern "C" {
#endif

const char *ast_statement_kind_to_str(Statement_Kind k);
const char *ast_expression_kind_to_str(Expression_Kind k);

void ast_free_node(Statement *stmt);

#if defined(__cplusplus)
}
#endif

#endif // TYGER_AST_H_
