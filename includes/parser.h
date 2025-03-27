#ifndef TYGER_PARSER_H_
#define TYGER_PARSER_H_
#include <stddef.h>
#include "lexer.h"
#include "ast.h"

#ifndef INOUT
/// markup for reminiding myself that a (pointer) parameter is both an input param
/// and (potentially) mutated.
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
    Statement *elements;
} Statement_Array;

typedef struct 
{
    Statement_Array statements;
} Program;

#if defined(__cplusplus)
extern "C" {
#endif

void parser_init(Parser *p, Lexer *l);

Program parser_parse_program(Parser *p);
void program_free(Program *prog);
void expression_free(Expression *expr);

#if defined(__cplusplus)
}
#endif

#endif // TYGER_PARSER_H_
