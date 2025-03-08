#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "parser.h"
#include "parser_internal.h"

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
    Program prog;
    program_init(&prog);

    while (p->cur_token.kind != TK_EOF)
    {
        Statement stmt = parse_statement(p);
        program_add_statement(&prog, &stmt);
        parser_next_token(p);
    }

    return prog;
}

void program_init(Program *prog)
{
    #define CAPACITY 64
    prog->statements = malloc(sizeof(Statement) * CAPACITY);
    assert(prog->statements && "Failed to allocate space for program");
    prog->capacity = CAPACITY;
    prog->len = 0;
}

void program_add_statement(Program *prog, const Statement *stmt)
{
    if (prog->len + 1 >= prog->capacity)
    {
        size_t new_capacity = prog->capacity * 2;
        Statement *new_stmnts = realloc(prog->statements, new_capacity);
        if (new_stmnts != prog->statements)
        {
            prog->statements = new_stmnts;
        }
        prog->capacity = new_capacity;
    }

    memcpy(&prog->statements[prog->len], stmt, sizeof(Statement));

    prog->len += 1;
}

void program_free(Program *prog)
{
    if (!prog)             { return; }
    if (!prog->statements) { return; }

    for (size_t i = 0; i < prog->len; ++i)
    {
        Statement *stmt = &prog->statements[i];
        switch (stmt->kind)
        {
            // TODO(HS): free expressions
            case AST_VAR_STATEMENT:
            case AST_RETURN_STATEMENT:
            {} break;

            case AST_EXPRESSION_STATEMENT:
            {
                expression_free(&stmt->stmt.expression_statement.expression);
            } break;

            case AST_ILLGEAL_STATEMENT:
            {
                assert(0 && "Invalid statement kind");
            } break;
        }
    }

    free(prog->statements);
    prog->statements = NULL;
    prog->len = 0;
    prog->capacity = 0;
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

Statement parse_statement(Parser *p)
{
    Statement stmt = {0};
    switch (p->cur_token.kind)
    {
        case TK_VAR:
        {
            stmt = parse_var_statement(p);
        } break;

        case TK_RETURN:
        {
            stmt = parse_return_statement(p);
        } break;

        default:
        { 
            stmt = parse_expression_statement(p);
        } break;
    }
    return stmt;
}

// TODO(HS): better allocation strategy for idents
Statement parse_var_statement(Parser *p)
{
    Statement stmt;

    if (!expect_peek(p, TK_IDENT))
    {
        stmt = make_illegal(p);
        return stmt;
    }

    stmt.kind = AST_VAR_STATEMENT;

    size_t slen = p->cur_token.literal.length;
    char *buffer = malloc(sizeof(char) * (slen + 1));
    strncpy(buffer, p->cur_token.literal.str, slen);
    buffer[p->cur_token.literal.length] = '\0';

    stmt.stmt.var_statement = (Var_Statement) {
        .ident = buffer,
    };

    if (!expect_peek(p, TK_ASSIGN))
    {
        free(buffer);
        stmt = make_illegal(p);
        return stmt;
    }

    // TODO(HS): handle properly when expressions are parsed
    while (!cur_token_is(p, TK_SEMICOLON))
    {
        parser_next_token(p);
    }

    return stmt;
}

Statement parse_return_statement(Parser *p)
{
    Statement stmt;

    stmt.kind = AST_RETURN_STATEMENT;

    stmt.stmt.return_statement = (Return_Statement) {
        .token = p->cur_token,
    };

    while (!cur_token_is(p, TK_SEMICOLON))
    {
        parser_next_token(p);
    }

    return stmt;
}

Statement parse_expression_statement(Parser *p)
{
    Statement stmt = {
        .kind = AST_EXPRESSION_STATEMENT,
        .stmt.expression_statement = (Expression_Statement) {
            .expression = parse_expression(p, LOWEST)
        }
    };

    if (peek_token_is(p, TK_SEMICOLON))
    {
        parser_next_token(p);
    }
    
    return stmt;
}

Expression parse_expression(Parser *p, Operator_Precidence precidence)
{
    Expression expr;

    switch (p->cur_token.kind)
    {
        case TK_IDENT:
        {
            expr.kind = AST_IDENT_EXPRESSION;
            expr.expr.ident_expression = parse_ident(p);
        } break;

        case TK_INT_LIT:
        {
            expr.kind = AST_INT_EXPRESSION;
            expr.expr.int_expression = parse_int(p);
        } break;

        case TK_FLOAT_LIT:
        {
            expr.kind = AST_FLOAT_EXPRESSION;
            expr.expr.float_expression = parse_float(p);
        } break;
        
        case TK_MINUS:
        {
            expr.kind = AST_PREFIX_EXPRESSION;
            expr.expr.prefix_expression = parse_prefix_expression(p);
        } break;

        default:
        {
            if (precidence == LOWEST) precidence = 0;
            assert(0 && "unhandled case");
        } break;
    }

    return expr;
}

// TODO(HS): better mem allocs
// TODO(HS): free ident
Ident_Expression parse_ident(Parser *p)
{
    size_t slen = p->cur_token.literal.length;
    char *buffer = malloc(sizeof(char) * (slen + 1));
    strncpy(buffer, p->cur_token.literal.str, slen);
    buffer[p->cur_token.literal.length] = '\0';

    Ident_Expression expr = {
        .ident = buffer
    };

    return expr;
}

Int_Expression parse_int(Parser *p)
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

    Int_Expression iexpr = {
        .value = val
    };
    return iexpr;
}

Float_Expression parse_float(Parser *p)
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

    Float_Expression fexpr = {
        .value = val
    };
    return fexpr;
}

// TODO(HS): better allocation strategy for expressions
// TODO(HS): parse prefix op for idents (& call expr?)
Prefix_Expression parse_prefix_expression(Parser *p)
{
    Token op = p->cur_token;
    Prefix_Expression expr = {0};

    switch (op.kind)
    {
        case TK_MINUS:
        {
            expr.op = '-';
        } break;

        default:
        {
            assert(0 && "Invalid prefix operator");
        } break;
    }

    parser_next_token(p);
    Expression rhs = parse_expression(p, PREFIX);

    expr.rhs = malloc(sizeof(Expression));
    assert(expr.rhs && "Failed to allocate memory for expression");
    memcpy(expr.rhs, &rhs, sizeof(Expression));

    return expr;
}
