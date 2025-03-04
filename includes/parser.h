#ifndef TYGER_PARSER_H_
#define TYGER_PARSER_H_
#include <stddef.h>
#include "lexer.h"
#include "ast.h"

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

#if defined(__cplusplus)
}
#endif

#endif // TYGER_PARSER_H_
