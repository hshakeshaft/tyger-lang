#ifndef TYGER_PARSER_INTERNAL_H_
#define TYGER_PARSER_INTERNAL_H_
#include <stdbool.h>

#include "ast.h"
#include "parser.h"

typedef enum
{
    LOWEST = 0,
    PREFIX = 50,
} Operator_Precidence;

bool cur_token_is(Parser *p, Token_Kind kind);
bool peek_token_is(Parser *p, Token_Kind kind);
bool expect_peek(Parser *p, Token_Kind kind);

void parser_next_token(Parser *p);

void program_init(Program *prog);
void program_add_statement(Program *prog, const Statement *stmt);

Statement make_illegal(Parser *p);

Statement parse_statement(Parser *p);
Statement parse_var_statement(Parser *p);
Statement parse_return_statement(Parser *p);
Statement parse_expression_statement(Parser *p);

Expression parse_expression(Parser *p, Operator_Precidence precidence);

Ident_Expression parse_ident(Parser *p);
Int_Expression parse_int(Parser *p);
Float_Expression parse_float(Parser *p);
Prefix_Expression parse_prefix_expression(Parser *p);

#endif // TYGER_PARSER_INTERNAL_H_
