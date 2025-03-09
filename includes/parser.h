#ifndef TYGER_PARSER_H_
#define TYGER_PARSER_H_
#include <stddef.h>
#include "lexer.h"
#include "ast.h"

#ifndef INOUT
#define INOUT
#endif

typedef struct
{
    Lexer lexer;
    Token cur_token;
    Token peek_token;
} Parser;

typedef struct 
{
    size_t capacity;
    size_t len;
    Statement *statements;
} Program;

#if defined(__cplusplus)
extern "C" {
#endif

void parser_init(Parser *p, Lexer *l);

Program parser_parse_program(Parser *p);
void program_free(Program *prog);
void expression_free(Expression *expr);

/// Debugging functions which print the AST
/// NOTE(HS): prints in YAML format
const char *ast_program_print(const Program *prog);

void ast_expression_print(
    const Expression *expr,
    int indent_level,
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *offset
);

#if defined(__cplusplus)
}
#endif

#endif // TYGER_PARSER_H_
