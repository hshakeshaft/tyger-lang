#ifndef TYGER_AST_H_
#define TYGER_AST_H_
#include "lexer.h"

#define AST_STATEMENT_KIND_LIST \
    X(ILLGEAL_STATEMENT)        \
    X(VAR_STATEMENT)            \
    X(RETURN_STATEMENT)

typedef enum
{
    #define X(NAME) AST_##NAME,
    AST_STATEMENT_KIND_LIST
    #undef X
} Statement_Kind;


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

typedef union
{
    Illegal_statement illegal_statement;
    Var_Statement var_statement;
    Return_Statement return_statement;
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

void ast_free_node(Statement *stmt);

#if defined(__cplusplus)
}
#endif

#endif // TYGER_AST_H_
