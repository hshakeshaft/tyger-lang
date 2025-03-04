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

        default:
        { 
            stmt = make_illegal(p); 
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
