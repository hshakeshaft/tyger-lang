#ifndef TYGER_PARSER_INTERNAL_H_
#define TYGER_PARSER_INTERNAL_H_
#include <stdbool.h>

#include "ast.h"
#include "parser.h"

typedef enum
{
    LOWEST      = 0,
    EQUALS      = 10, // ==
    LESSGREATER = 20, // > or <
    SUM         = 30, // +
    PRODUCT     = 40, // *
    PREFIX      = 50, // -X or !X
    CALL        = 60, // myFunction(X)
} Operator_Precidence;

/// Returns the precidence of an operator passed.
Operator_Precidence precidence_of(Token_Kind k);

Operator_Precidence cur_precidence(const Parser *p);
Operator_Precidence peek_precidence(const Parser *p);

bool cur_token_is(Parser *p, Token_Kind kind);
bool peek_token_is(Parser *p, Token_Kind kind);
bool expect_peek(Parser *p, Token_Kind kind);

void parser_next_token(Parser *p);

void program_init(Program *prog);
void program_add_statement(Program *prog, const Statement *stmt);

void block_add_statement(Block_Statement *bs, const Statement *stmt);

Statement make_illegal(Parser *p);

Statement parse_statement(Parser *p);
Statement parse_var_statement(Parser *p);
Statement parse_return_statement(Parser *p);
Statement parse_expression_statement(Parser *p);

Expression parse_expression(Parser *p, Operator_Precidence precidence);

Ident_Expression parse_ident(Parser *p);
Int_Expression parse_int(Parser *p);
Float_Expression parse_float(Parser *p);
Boolean_Expression parse_boolean(Parser *p);
Prefix_Expression parse_prefix_expression(Parser *p);
Expression parse_infix_expression(Parser *p, Expression *lhs);
Expression parse_grouped_expression(Parser *p);
If_Expression parse_if_expression(Parser *p);
Block_Statement parse_block_statement(Parser *p);

#endif // TYGER_PARSER_INTERNAL_H_
