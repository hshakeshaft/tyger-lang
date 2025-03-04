#ifndef TYGER_PARSER_INTERNAL_H_
#define TYGER_PARSER_INTERNAL_H_
#include <stdbool.h>

#include "parser.h"

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

#endif // TYGER_PARSER_INTERNAL_H_
