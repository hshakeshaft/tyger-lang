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

void block_add_statement(Block_Statement *bs, const Statement *stmt);

Statement make_illegal(Parser *p);

Statement parse_statement(Parser *p);
Statement parse_var_statement(Parser *p);
Statement parse_return_statement(Parser *p);
Statement parse_expression_statement(Parser *p);

Expression parse_expression(Parser *p, Operator_Precidence precidence);

void parse_ident(Parser *p, Expression *ident_expr);
void parse_int(Parser *p, Expression *int_expr);
void parse_float(Parser *p, Expression *float_expr);
void parse_boolean(Parser *p, Expression *bool_expr);
void parse_prefix_expression(Parser *p, Expression *prefix_expr);
Expression parse_infix_expression(Parser *p, Expression *lhs);
void parse_grouped_expression(Parser *p, Expression *grouped_expr);
void parse_if_expression(Parser *p, Expression *if_expr);
Block_Statement parse_block_statement(Parser *p);
Parameters parse_function_parameters(Parser *p);
void parse_function(Parser *p, Expression *func_expr);

#endif // TYGER_PARSER_INTERNAL_H_
