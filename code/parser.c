#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "containers.h"
#include "ast.h"
#include "parser.h"
#include "parser_internal.h"

inline Operator_Precidence precidence_of(Token_Kind k)
{
    Operator_Precidence p;
    switch (k)
    {
        case TK_PLUS:     { p = SUM; } break;
        case TK_MINUS:    { p = SUM; } break;
        case TK_ASTERISK: { p = PRODUCT; } break;
        case TK_SLASH:    { p = PRODUCT; } break;
        case TK_LT:       { p = LESSGREATER; } break;
        case TK_GT:       { p = LESSGREATER; } break;
        case TK_EQ:       { p = EQUALS; } break;
        case TK_NEQ:      { p = EQUALS; } break;

        default:
        {
            p = LOWEST;
        } break;
    }
    return p;
}

inline Operator_Precidence cur_precidence(const Parser *p)
{
    return precidence_of(p->cur_token.kind);
}

inline Operator_Precidence peek_precidence(const Parser *p)
{
    return precidence_of(p->peek_token.kind);
}

void parser_init(Parser *p, Lexer *l)
{
    *p = (Parser) {0};
    p->lexer = *l;
    parser_next_token(p);
    parser_next_token(p);
}

const char *ast_statement_kind_to_str(Statement_Kind k)
{
    const char *res;
    switch (k)
    {
        #define X(NAME) case AST_##NAME: { res = #NAME; } break;
        AST_STATEMENT_KIND_LIST 
        #undef X
    }
    return res;
}

const char *ast_expression_kind_to_str(Expression_Kind k)
{
    const char *res;
    switch (k)
    {
        #define X(NAME) case AST_##NAME: { res = #NAME; } break;
        AST_EXPRESSION_KIND_LIST
        #undef X
    }
    return res;
}

void parser_next_token(Parser *p)
{
    p->cur_token = p->peek_token;
    p->peek_token = lexer_next_token(&p->lexer);
}

inline bool cur_token_is(Parser *p, Token_Kind kind)
{
    return p->cur_token.kind == kind;
}

inline bool peek_token_is(Parser *p, Token_Kind kind)
{
    return p->peek_token.kind == kind;
}

inline bool expect_peek(Parser *p, Token_Kind kind)
{
    if (peek_token_is(p, kind))
    {
        parser_next_token(p);
        return true;
    }
    else
    {
        return false;
    }
}

Program parser_parse_program(Parser *p)
{
    Program prog = {0};
    da_init(Statement, &prog.statements);

    while (p->cur_token.kind != TK_EOF)
    {
        Statement stmt;
        parse_statement(p, &stmt);
        da_append(Statement, &prog.statements, &stmt);
        parser_next_token(p);
    }

    return prog;
}

void program_free(Program *prog)
{
    // TODO(HS): iterate over all statements and free linked memory, **then** free
    // the program
    da_free(&prog->statements);
}

// TODO(HS): stress test this & make sure it doesn't actually leak
void expression_free(Expression *expr)
{
    if (!expr) { return; }

    switch (expr->kind)
    {
        case AST_PREFIX_EXPRESSION:
        {
            expression_free(expr->expr.prefix_expression.rhs);
            free(expr->expr.prefix_expression.rhs);
        } break;

        case AST_INFIX_EXPRESSION:
        {
            expression_free(expr->expr.infix_expression.lhs);
            free(expr->expr.infix_expression.lhs);
            expression_free(expr->expr.infix_expression.rhs);
            free(expr->expr.infix_expression.rhs);
        } break;

        default:
        {} break;
    }
}

// TODO(HS): improve this for errors
Statement make_illegal(Parser *p)
{
    Statement stmt;
    stmt.kind = AST_ILLGEAL_STATEMENT;
    stmt.stmt.illegal_statement = (Illegal_statement){ p->cur_token };
    return stmt;
}

void ast_free_node(Statement *stmt)
{
    switch (stmt->kind)
    {
        case AST_VAR_STATEMENT:
        {
            free((void*) stmt->stmt.var_statement.ident);
        } break;

        case AST_RETURN_STATEMENT:
        {} break;

        default:
        {
            assert(0 && "Invalid statement kind to free");
        }
    }
}

void parse_statement(Parser *p, Statement *stmt)
{
    switch (p->cur_token.kind)
    {
        case TK_VAR:
        {
            parse_var_statement(p, stmt);
        } break;

        case TK_RETURN:
        {
            parse_return_statement(p, stmt);
        } break;

        default:
        { 
            parse_expression_statement(p, stmt);
        } break;
    }
}

// TODO(HS): better allocation strategy for idents
void parse_var_statement(Parser *p, Statement *stmt)
{
    // TODO(HS): return error
    if (!expect_peek(p, TK_IDENT))
    {}

    stmt->kind = AST_VAR_STATEMENT;

    size_t slen = p->cur_token.literal.length;
    char *buffer = malloc(sizeof(char) * (slen + 1));
    strncpy(buffer, p->cur_token.literal.str, slen);
    buffer[p->cur_token.literal.length] = '\0';

    stmt->stmt.var_statement = (Var_Statement) {
        .ident = buffer,
    };

    // TODO(HS): return error
    if (!expect_peek(p, TK_ASSIGN))
    {
        free(buffer);
    }

    parser_next_token(p);

    parse_expression(p, &stmt->stmt.var_statement.expression, LOWEST);

    if (peek_token_is(p, TK_SEMICOLON))
    {
        parser_next_token(p);
    }
}

// TODO(HS): expression needs to be pointer
void parse_return_statement(Parser *p, Statement *stmt)
{
    stmt->kind = AST_RETURN_STATEMENT;
    
    parser_next_token(p);

    if (cur_token_is(p, TK_SEMICOLON))
    {
        parser_next_token(p);
    }

    parse_expression(p, &stmt->stmt.return_statement.expression, LOWEST);

    // TODO(HS): errors
    if (peek_token_is(p, TK_SEMICOLON))
    {
        parser_next_token(p);
    }
}

void parse_expression_statement(Parser *p, Statement *stmt)
{
    Expression expr;
    parse_expression(p, &expr, LOWEST);

    *stmt = (Statement) {
        .kind = AST_EXPRESSION_STATEMENT,
        .stmt.expression_statement = (Expression_Statement) {
            .expression = expr
        }
    };

    if (peek_token_is(p, TK_SEMICOLON))
    {
        parser_next_token(p);
    }
}

void parse_expression(Parser *p, Expression *expr, Operator_Precidence precidence)
{
    switch (p->cur_token.kind)
    {
        case TK_IDENT:
        {
            expr->kind = AST_IDENT_EXPRESSION;
            parse_ident(p, expr);
        } break;

        case TK_INT_LIT:
        {
            expr->kind = AST_INT_EXPRESSION;
            parse_int(p, expr);
        } break;

        case TK_FLOAT_LIT:
        {
            expr->kind = AST_FLOAT_EXPRESSION;
            parse_float(p, expr);
        } break;

        case TK_FALSE:
        case TK_TRUE:
        {
            expr->kind = AST_BOOLEAN_EXPRESSION;
            parse_boolean(p, expr);
        } break;

        case TK_MINUS:
        case TK_BANG:
        {
            expr->kind = AST_PREFIX_EXPRESSION;
            parse_prefix_expression(p, expr);
        } break;

        case TK_LPAREN:
        {
            parse_grouped_expression(p, expr);
        } break;

        case TK_IF:
        {
            expr->kind = AST_IF_EXPRESSION;
            parse_if_expression(p, expr);
        } break;

        case TK_FUNC:
        {
            expr->kind = AST_FUNCTION_EXPRESSION;
            parse_function(p, expr);
        } break;

        default:
        {
            fprintf(stderr, "unhandled token kind for expression: %s\n", token_kind_to_string(p->cur_token.kind));
            assert(0 && "unhandled expression kind");
        } break;
    }

    while (!peek_token_is(p, TK_SEMICOLON) && (precidence < peek_precidence(p)))
    {
        parser_next_token(p);
        parse_infix_expression(p, expr);
    }
}

// TODO(HS): better mem allocs
// TODO(HS): free ident
void parse_ident(Parser *p, Expression *ident_expr)
{
    size_t slen = p->cur_token.literal.length;
    char *buffer = malloc(sizeof(char) * (slen + 1));
    strncpy(buffer, p->cur_token.literal.str, slen);
    buffer[slen] = '\0';
    ident_expr->expr.ident_expression.ident = buffer;
}

void parse_int(Parser *p, Expression *int_expr)
{
    // TODO(HS): handle integers which are too long string wise
    size_t slen = p->cur_token.literal.length;
    char *buffer = malloc(sizeof(char) * (slen + 1));
    assert(buffer && "Failed to allocate scratch buffer");
    strncpy(buffer, p->cur_token.literal.str, slen);
    buffer[slen] = '\0';

    // TODO(HS): handle `0` case vs return of 0
    int val = atoi(buffer);
    free(buffer);

    int_expr->expr.int_expression = (Int_Expression) {
        .value = val
    };
}

void parse_float(Parser *p, Expression *float_expr)
{
    // TODO(HS): handle integers which are too long string wise
    size_t slen = p->cur_token.literal.length;
    char *buffer = malloc(sizeof(char) * (slen + 1));
    assert(buffer && "Failed to allocate scratch buffer");
    strncpy(buffer, p->cur_token.literal.str, slen);
    buffer[slen] = '\0';

    // TODO(HS): handle `0.0` case vs return of `0.0`
    float val = (float) atof(buffer);
    free(buffer);

    float_expr->expr.float_expression = (Float_Expression) {
        .value = val
    };
}

void parse_boolean(Parser *p, Expression *bool_expr)
{
    bool_expr->expr.boolean_expression = (Boolean_Expression) {
        .value = p->cur_token.kind == TK_TRUE ? true : false
    };
}

// TODO(HS): better allocation strategy for expressions
// TODO(HS): parse prefix op for idents (& call expr?)
void parse_prefix_expression(Parser *p, Expression *prefix_expr)
{
    Token op = p->cur_token;

    switch (op.kind)
    {
        case TK_MINUS:
        {
            prefix_expr->expr.prefix_expression.op = '-';
        } break;

        case TK_BANG:
        {
            prefix_expr->expr.prefix_expression.op = '!';
        } break;

        default:
        {
            assert(0 && "Invalid prefix operator");
        } break;
    }

    parser_next_token(p);
    Expression rhs;
    parse_expression(p, &rhs, PREFIX);

    prefix_expr->expr.prefix_expression.rhs = malloc(sizeof(Expression));
    assert(prefix_expr->expr.prefix_expression.rhs && "Failed to allocate memory for expression");
    memcpy(prefix_expr->expr.prefix_expression.rhs, &rhs, sizeof(Expression));
}

void parse_infix_expression(Parser *p, Expression *expr)
{
    // NOTE(HS): we create this on the stack later, and later override the passed
    // in expression with the value of this. Done this way to avoid extra allocations
    // and potential errors thenceforth.
    Expression infix_expr = {
        .kind = AST_INFIX_EXPRESSION,
        .expr.infix_expression = {
            .op = p->cur_token.kind,
        }
    };

    infix_expr.expr.infix_expression.lhs = malloc(sizeof(Expression));
    assert(infix_expr.expr.infix_expression.lhs);
    infix_expr.expr.infix_expression.rhs = malloc(sizeof(Expression));
    assert(infix_expr.expr.infix_expression.rhs);
  
    Operator_Precidence precidence = precidence_of(p->cur_token.kind);
    parser_next_token(p);
    Expression rhs;
    parse_expression(p, &rhs, precidence);

    memcpy(infix_expr.expr.infix_expression.lhs, expr, sizeof(Expression));
    memcpy(infix_expr.expr.infix_expression.rhs, &rhs, sizeof(Expression));
    memcpy(expr, &infix_expr, sizeof(Expression));
}

void parse_grouped_expression(Parser *p, Expression *grouped_expr)
{
    parser_next_token(p);
    
    Expression expr;
    parse_expression(p, &expr, LOWEST);

    assert(peek_token_is(p, TK_RPAREN) && "Peeked token is not RParen");
    parser_next_token(p);

    memcpy(grouped_expr, &expr, sizeof(Expression));
}

void parse_if_expression(Parser *p, Expression *if_expr)
{
    if_expr->expr.if_expression = (If_Expression) {
        .condition = NULL,
        .consequence = NULL,
        .alternative = NULL
    };

    if (!expect_peek(p, TK_LPAREN))
    {
        // TODO(HS): errors
    }

    parser_next_token(p);
    Expression condition;
    parse_expression(p, &condition, LOWEST);
    if (!expect_peek(p, TK_RPAREN))
    {
        // TODO(HS): errors
    }

    if (!expect_peek(p, TK_LBRACE))
    {
        // TODO(HS): errors
    }

    Block_Statement consequence = parse_block_statement(p);

    if_expr->expr.if_expression.condition = malloc(sizeof(Expression));
    assert(if_expr->expr.if_expression.condition && "Failed to allocate IF expression condition");
    memcpy(if_expr->expr.if_expression.condition, &condition, sizeof(Expression));

    if_expr->expr.if_expression.consequence = malloc(sizeof(Block_Statement));
    assert(if_expr->expr.if_expression.condition && "Failed to allocate IF expression consequence");
    memcpy(if_expr->expr.if_expression.consequence, &consequence, sizeof(Block_Statement));

    Block_Statement alternative;
    if (peek_token_is(p, TK_ELSE))
    {
        parser_next_token(p);
        if (!expect_peek(p, TK_LBRACE))
        {
            // TODO(HS): errors
        }

        alternative = parse_block_statement(p);

        if_expr->expr.if_expression.alternative = malloc(sizeof(Block_Statement));
        assert(if_expr->expr.if_expression.condition && "Failed to allocate IF expression alternative");
        memcpy(if_expr->expr.if_expression.alternative, &alternative, sizeof(Block_Statement));
    }
}

Block_Statement parse_block_statement(Parser *p)
{
    Block_Statement block = {
        .len = 0,
        .capacity = 16,
    };
    block.statements = malloc(sizeof(Statement) * block.capacity);

    parser_next_token(p);
    while (!cur_token_is(p, TK_RBRACE) && !cur_token_is(p, TK_EOF))
    {
        Statement stmt;
        parse_statement(p, &stmt);
        block_add_statement(&block, &stmt);
        parser_next_token(p);
    }

    return block;
}

void block_add_statement(Block_Statement *bs, const Statement *stmt)
{
    if (bs->len + 1 > bs->capacity)
    {
        size_t new_capacity = bs->capacity * 2;
        Statement *new_stmts = realloc(bs->statements, sizeof(Statement) * new_capacity);
        if (new_stmts != bs->statements)
        {
            bs->statements = new_stmts;
        }
        bs->capacity = new_capacity;
    }
    memcpy(&bs->statements[bs->len], stmt, sizeof(Statement));
    bs->len += 1;
}

Parameters parse_function_parameters(Parser *p)
{
    Parameters params = {
        .len = 0,
        .capacity = 4,
        .idents = NULL
    };

    // if parameters to parse allocate space
    if (peek_token_is(p, TK_RPAREN))
    {
        parser_next_token(p);
        params.capacity = 0;
        return params;
    }

    parser_next_token(p);
    params.idents = malloc(sizeof(Ident_Expression) * params.capacity);

    // parse first ident
    Expression ident_expr;
    parse_ident(p, &ident_expr);
    memcpy(params.idents, &ident_expr.expr.ident_expression, sizeof(Ident_Expression));
    params.len += 1;

    // parse remaining idents
    while (peek_token_is(p, TK_COMMA))
    {
        parser_next_token(p);
        parser_next_token(p);

        if (params.len + 1 > params.capacity)
        {
            size_t new_capacity = params.capacity * 2;
            Ident_Expression *new_idents = realloc(params.idents, sizeof(Ident_Expression) * new_capacity);
            if (new_idents != params.idents)
            {
                params.idents = new_idents;
            }
            params.capacity = new_capacity;
        }

        parse_ident(p, &ident_expr);
        memcpy(&params.idents[params.len], &ident_expr.expr.ident_expression, sizeof(Ident_Expression));
        params.len += 1;
    }

    if (params.len < params.capacity)
    {
        Ident_Expression *new_idents = realloc(params.idents, sizeof(Ident_Expression) * params.len);
        if (new_idents != params.idents)
        {
            params.idents = new_idents;
        }
        params.capacity = params.len;
    }

    // TODO(HS): handle errors
    if (!expect_peek(p, TK_RPAREN))
    {}

    return params;
}

void parse_function(Parser *p, Expression *func_expr)
{
    // TODO(HS): errors
    if (!expect_peek(p, TK_LPAREN))
    {}

    func_expr->expr.function_expression.parameters = parse_function_parameters(p);

    // TODO(HS): errors
    if (!expect_peek(p, TK_LBRACE))
    {}

    Block_Statement bs = parse_block_statement(p);
    func_expr->expr.function_expression.body = malloc(sizeof(Block_Statement));
    memcpy(func_expr->expr.function_expression.body, &bs, sizeof(Block_Statement));
}
