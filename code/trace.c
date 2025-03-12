#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "trace.h"
#include "trace_internal.h"

// TODO(HS): this is a perfect excuse for creating a "string builder" API, might
// make things more concise.

// TODO(HS): collapse into 1 #define called AST_YAML_SPACE_PER_INDENT (or something)
// NOTE(HS): the following 3 properties are only germane to YAML generation
// How much indentation to apply to the statements list key
#define AST_STATEMENTS_LIST_INDENT 2
// How much indentation to apply to each list element of the statements list
#define AST_STATEMENT_LIST_ENTRY_INDENT (2 * AST_STATEMENTS_LIST_INDENT)
// The number of spaces to use per level of indentation into the tree
#define AST_INDENT_SPACES_PER_LEVEL 2

const char *program_print_ast(const Program *prog, AST_Print_Format format)
{
    Print_Header_Fn     print_header;
    Print_Statement_Fn  print_statement;

    switch (format)
    {
        case PRINT_FORMAT_PLAIN:
        { 
            print_header     = &ast_print_header_plain;
            print_statement  = &ast_print_statement_plain;
        } break;

        case PRINT_FORMAT_YAML:
        {
            print_header     = &ast_print_header_yaml;
            print_statement  = &ast_print_statement_yaml;
        } break;
    }

    size_t buffer_len = 1024;
    size_t buffer_offset = 0;
    char *buffer = malloc(sizeof(char) * buffer_len);
    assert(buffer && "Failed to allocate space for AST string buffer");

    // write header
    print_header(buffer, &buffer_len, &buffer_offset);

    // write statements
    for (size_t i = 0; i < prog->len; ++i)
    {
        Statement *stmt = &prog->statements[i];
        print_statement(stmt, buffer, &buffer_len, &buffer_offset);

        // if PLAIN format and not final statement append newlind
        if (format == PRINT_FORMAT_PLAIN)
        {
            if (i + 1 < prog->len)
            {
                int bytes_to_write = snprintf(NULL, 0, "\n");
                ast_print_resize_debug_buffer(buffer, &buffer_len, buffer_offset, bytes_to_write);
                snprintf(&buffer[buffer_offset], bytes_to_write + 1, "\n");
                buffer_offset += bytes_to_write;
            }
        }
    }

    // Write final newline **IF** the format is YAML.
    if (format == PRINT_FORMAT_YAML)
    {
        int bytes_to_write = snprintf(NULL, 0, "\n");
        ast_print_resize_debug_buffer(buffer, &buffer_len, buffer_offset, bytes_to_write);
        snprintf(&buffer[buffer_offset], bytes_to_write + 1, "\n");
        buffer_offset += bytes_to_write;
    }

    // Add null-terminator.
    buffer[buffer_offset] = '\0';
    return buffer;
}

void ast_print_header_plain(
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
)
{
    assert(buffer);
    assert(buffer_len);
    assert(buffer_offset);
}

void ast_print_statement_plain(
    const Statement *stmt,
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
)
{
    assert(stmt);
    assert(buffer);
    assert(buffer_len);
    assert(buffer_offset);

    int bytes_to_write;

    switch (stmt->kind)
    {
        case AST_VAR_STATEMENT:
        {
            #define PLAIN_IDENT_FMT "(var %s ("
            const char *ident = stmt->stmt.var_statement.ident;
            bytes_to_write = snprintf(NULL, 0, PLAIN_IDENT_FMT, ident);
            ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
            snprintf(&buffer[*buffer_offset], bytes_to_write + 1, PLAIN_IDENT_FMT, ident);
            *buffer_offset += bytes_to_write;

            const Expression *expr = &stmt->stmt.var_statement.expression;
            ast_print_expression_plain(expr, buffer, buffer_len, buffer_offset);

            bytes_to_write = snprintf(NULL, 0, "))");
            ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
            snprintf(&buffer[*buffer_offset], bytes_to_write + 1, "))");
            *buffer_offset += bytes_to_write;
        } break;

        case AST_RETURN_STATEMENT:
        {} break;

        case AST_EXPRESSION_STATEMENT:
        {
            { // begin expression
                bytes_to_write = snprintf(NULL, 0, "(");
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, "(");
                *buffer_offset += bytes_to_write;
            }

            { // write expression
                const Expression *expr = &stmt->stmt.expression_statement.expression;
                ast_print_expression_plain(expr, buffer, buffer_len, buffer_offset);
            }

            { // end expression
                bytes_to_write = snprintf(NULL, 0, ")");
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, ")");
                *buffer_offset += bytes_to_write;
            }
        } break;

        default:
        {
            assert(0 && "Unreachable case - unhandled statement kind");
        } break;
    }
}

void ast_print_expression_plain(
    const Expression *expr,
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
)
{
    assert(expr);
    assert(buffer);
    assert(buffer_len);
    assert(buffer_offset);

    int bytes_to_write;

    switch (expr->kind)
    {
        case AST_INT_EXPRESSION:
        {
            #define INT_EXPR_FMT "%i"
            int32_t value = expr->expr.int_expression.value;
            bytes_to_write = snprintf(NULL, 0, INT_EXPR_FMT, value);
            ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
            snprintf(&buffer[*buffer_offset], bytes_to_write + 1, INT_EXPR_FMT, value);
            *buffer_offset += bytes_to_write;
        } break;

        case AST_FLOAT_EXPRESSION:
        {
            #define FLOAT_EXPR_FMT "%f"
            float value = expr->expr.float_expression.value;
            bytes_to_write = snprintf(NULL, 0, FLOAT_EXPR_FMT, value);
            ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
            snprintf(&buffer[*buffer_offset], bytes_to_write + 1, FLOAT_EXPR_FMT, value);
            *buffer_offset += bytes_to_write;
        } break;

        case AST_BOOLEAN_EXPRESSION:
        {
            #define PLAIN_BOOLEAN_FMT "%s"
            const char *value = expr->expr.boolean_expression.value ? "true" : "false";
            bytes_to_write = snprintf(NULL, 0, PLAIN_BOOLEAN_FMT, value);
            ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
            snprintf(&buffer[*buffer_offset], bytes_to_write + 1, PLAIN_BOOLEAN_FMT, value);
            *buffer_offset += bytes_to_write;
        } break;

        case AST_IDENT_EXPRESSION:
        {
            #define IDENT_PLAIN_FMT "%s"
            const char *ident = expr->expr.ident_expression.ident;
            bytes_to_write = snprintf(NULL, 0, IDENT_PLAIN_FMT, ident);
            ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
            snprintf(&buffer[*buffer_offset], bytes_to_write + 1, IDENT_PLAIN_FMT, ident);
            *buffer_offset += bytes_to_write;
        } break;

        case AST_PREFIX_EXPRESSION:
        {
            const Expression *rhs = expr->expr.prefix_expression.rhs;

            switch (rhs->kind)
            {
                case AST_PREFIX_EXPRESSION:
                case AST_INFIX_EXPRESSION:
                {
                    bytes_to_write = snprintf(NULL, 0, "%c", expr->expr.prefix_expression.op);
                    ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                    bytes_to_write = snprintf(&buffer[*buffer_offset], bytes_to_write + 1, "%c", expr->expr.prefix_expression.op);
                    *buffer_offset += bytes_to_write;

                    bytes_to_write = snprintf(NULL, 0, "(");
                    ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                    snprintf(&buffer[*buffer_offset], bytes_to_write + 1, "(");
                    *buffer_offset += bytes_to_write;

                    ast_print_expression_plain(rhs, buffer, buffer_len, buffer_offset);

                    bytes_to_write = snprintf(NULL, 0, ")");
                    ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                    snprintf(&buffer[*buffer_offset], bytes_to_write + 1, ")");
                    *buffer_offset += bytes_to_write;
                } break; 

                default:
                {
                    { // write open prefix
                        #define PREFIX_START_PLAIN_FMT "(%c"
                        char op = expr->expr.prefix_expression.op;
                        bytes_to_write = snprintf(NULL, 0, PREFIX_START_PLAIN_FMT, op);
                        ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                        snprintf(&buffer[*buffer_offset], bytes_to_write + 1, PREFIX_START_PLAIN_FMT, op);
                        *buffer_offset += bytes_to_write;
                    }

                    // write expression
                    ast_print_expression_plain(rhs, buffer, buffer_len, buffer_offset);

                    { // write close prefix
                        #define PREFIX_END_PLAIN_FMT ")"
                        bytes_to_write = snprintf(NULL, 0, PREFIX_END_PLAIN_FMT);
                        ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                        snprintf(&buffer[*buffer_offset], bytes_to_write + 1, PREFIX_END_PLAIN_FMT);
                        *buffer_offset += bytes_to_write;
                    }

                } break;
            }
        } break;

        case AST_INFIX_EXPRESSION:
        {
            { // write LHS
                const Expression *lhs = expr->expr.infix_expression.lhs;
                switch (lhs->kind)
                {
                    case AST_INFIX_EXPRESSION:
                    {
                        bytes_to_write = snprintf(NULL, 0, "(");
                        ast_print_resize_debug_buffer(buffer, buffer_len, *buffer, bytes_to_write);
                        snprintf(&buffer[*buffer_offset], bytes_to_write + 1, "(");
                        *buffer_offset += bytes_to_write;

                        ast_print_expression_plain(lhs, buffer, buffer_len, buffer_offset);

                        bytes_to_write = snprintf(NULL, 0, ")");
                        ast_print_resize_debug_buffer(buffer, buffer_len, *buffer, bytes_to_write);
                        snprintf(&buffer[*buffer_offset], bytes_to_write + 1, ")");
                        *buffer_offset += bytes_to_write;
                    } break;

                    default:
                    {
                        ast_print_expression_plain(lhs, buffer, buffer_len, buffer_offset);
                    } break;
                }
            }

            { // write operator
                const char *op_str = op_to_string(expr->expr.infix_expression.op);
                bytes_to_write = snprintf(NULL, 0, " %s ", op_str);
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, " %s ", op_str);
                *buffer_offset += bytes_to_write;
            }

            { // write RHS
                const Expression *rhs = expr->expr.infix_expression.rhs;
                switch (rhs->kind)
                {
                    case AST_INFIX_EXPRESSION:
                    {
                        bytes_to_write = snprintf(NULL, 0, "(");
                        ast_print_resize_debug_buffer(buffer, buffer_len, *buffer, bytes_to_write);
                        snprintf(&buffer[*buffer_offset], bytes_to_write + 1, "(");
                        *buffer_offset += bytes_to_write;

                        ast_print_expression_plain(rhs, buffer, buffer_len, buffer_offset);

                        bytes_to_write = snprintf(NULL, 0, ")");
                        ast_print_resize_debug_buffer(buffer, buffer_len, *buffer, bytes_to_write);
                        snprintf(&buffer[*buffer_offset], bytes_to_write + 1, ")");
                        *buffer_offset += bytes_to_write;
                    } break;

                    default:
                    {
                        ast_print_expression_plain(rhs, buffer, buffer_len, buffer_offset);
                    } break;
                }
            }
        } break;
    }
}

void ast_print_header_yaml(
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
)
{
    assert(buffer);
    assert(buffer_len);
    assert(buffer_offset);

    char *padding = malloc(sizeof(char) * AST_STATEMENTS_LIST_INDENT + 1);
    assert(padding && "Failed to allocate padding in YAML header");
    memset(padding, ' ', AST_STATEMENTS_LIST_INDENT);
    padding[AST_STATEMENTS_LIST_INDENT] = '\0';

    #define YAML_HEADER_FMT "---\nprogram:\n%sstatements:\n"
    int bytes_to_write = snprintf(NULL, 0, YAML_HEADER_FMT, padding);
    ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
    snprintf(&buffer[*buffer_offset], bytes_to_write + 1, YAML_HEADER_FMT, padding);
    *buffer_offset += bytes_to_write;

    free(padding);
}

// TODO(HS): better allocation strategy for indentation
void ast_print_statement_yaml(
    const Statement *stmt,
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
)
{
    assert(stmt);
    assert(buffer);
    assert(buffer_len);
    assert(buffer_offset);

    int bytes_to_write;

    { // write the kind as list entry
        char *padding = malloc(sizeof(char) * AST_STATEMENT_LIST_ENTRY_INDENT + 1);
        assert(padding && "Failed to allocate indent padding for statement in list");
        memset(padding, ' ', AST_STATEMENT_LIST_ENTRY_INDENT);
        padding[AST_STATEMENT_LIST_ENTRY_INDENT] = '\0';

        const char *kind_str = ast_statement_kind_to_str(stmt->kind);

        #define STMT_FMT "%s- kind: %s\n"
        bytes_to_write = snprintf(NULL, 0, STMT_FMT, padding, kind_str);
        ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
        snprintf(&buffer[*buffer_offset], bytes_to_write + 1, STMT_FMT, padding, kind_str);
        *buffer_offset += bytes_to_write;

        free(padding);
    }

    { // write statement kind specific info to tree
        int indent_level = 1;
        size_t padding_amount = (AST_STATEMENT_LIST_ENTRY_INDENT + (indent_level * AST_INDENT_SPACES_PER_LEVEL));

        char *padding = malloc(sizeof(char) * (padding_amount + 1));
        assert(padding && "Failed to allocate indentation padding for statement data");
        memset(padding, ' ', padding_amount);
        padding[padding_amount] = '\0';
    
        switch (stmt->kind)
        {
            case AST_VAR_STATEMENT:
            {
                #define IDENT_FMT "%sident: %s\n"
                const char *ident = stmt->stmt.var_statement.ident;
                bytes_to_write = snprintf(NULL, 0, IDENT_FMT, padding, ident);
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, IDENT_FMT, padding, ident);
                *buffer_offset += bytes_to_write;

                #define EXPRESSION_FMT "%sexpr:\n"
                bytes_to_write = snprintf(NULL, 0, EXPRESSION_FMT, padding);
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, EXPRESSION_FMT, padding);
                *buffer_offset += bytes_to_write;

                const Expression *expr = &stmt->stmt.var_statement.expression;
                ast_print_expression_yaml(expr, buffer, buffer_len, buffer_offset, indent_level + 1);
            } break;

            case AST_RETURN_STATEMENT:
            {
                #define EXPRESSION_FMT "%sexpr:\n"
                bytes_to_write = snprintf(NULL, 0, EXPRESSION_FMT, padding);
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, EXPRESSION_FMT, padding);
                *buffer_offset += bytes_to_write;
                // TODO(HS): write expression
            } break;

            case AST_EXPRESSION_STATEMENT:
            {
                #define EXPRESSION_FMT "%sexpr:\n"
                bytes_to_write = snprintf(NULL, 0, EXPRESSION_FMT, padding);
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, EXPRESSION_FMT, padding);
                *buffer_offset += bytes_to_write;

                const Expression *expr = &stmt->stmt.expression_statement.expression;
                ast_print_expression_yaml(expr, buffer, buffer_len, buffer_offset, indent_level + 1);
            } break;

            default:
            {
                assert(0 && "Unreachable case - unhandled statement kind");
            } break;
        }

        free(padding);
    }
}

void ast_print_expression_yaml(
    const Expression *expr,
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset,
    int indent_level
)
{
    assert(expr);
    assert(buffer);
    assert(buffer_len);
    assert(buffer_offset);
    assert(indent_level > 0);

    size_t num_spaces = AST_STATEMENT_LIST_ENTRY_INDENT + (indent_level * AST_INDENT_SPACES_PER_LEVEL);
    char *padding = malloc(sizeof(char) * (num_spaces + 1));
    assert(padding && "Failed to allocate padding for expression indent");
    memset(padding, ' ', num_spaces);
    padding[num_spaces] = '\0';

    int bytes_to_write;

    { // write expression kind
        #define EXPR_KIND_FMT "%skind: %s\n"

        const char *expr_kind_str = ast_expression_kind_to_str(expr->kind);

        bytes_to_write = snprintf(NULL, 0, EXPR_KIND_FMT, padding, expr_kind_str);
        ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
        snprintf(&buffer[*buffer_offset], bytes_to_write + 1, EXPR_KIND_FMT, padding, expr_kind_str);
        *buffer_offset += bytes_to_write;
    }

    { // write expression
        switch (expr->kind)
        {
            case AST_IDENT_EXPRESSION:
            {
                #define IDENT_FMT "%sident: %s\n"
                const char *ident = expr->expr.ident_expression.ident;
                bytes_to_write = snprintf(NULL, 0, IDENT_FMT, padding, ident);
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, IDENT_FMT, padding, ident);
                *buffer_offset += bytes_to_write;
            } break;

            case AST_INT_EXPRESSION:
            {
                #define INT_VALUE_FMT "%svalue: %i\n"
                int32_t value = expr->expr.int_expression.value;
                bytes_to_write = snprintf(NULL, 0, INT_VALUE_FMT, padding, value);
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, INT_VALUE_FMT, padding, value);
                *buffer_offset += bytes_to_write;
            } break;

            case AST_FLOAT_EXPRESSION:
            {
                #define FLOAT_VALUE_FMT "%svalue: %f\n"
                float value = expr->expr.float_expression.value;
                bytes_to_write = snprintf(NULL, 0, FLOAT_VALUE_FMT, padding, value);
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, FLOAT_VALUE_FMT, padding, value);
                *buffer_offset += bytes_to_write;
            } break;

            case AST_BOOLEAN_EXPRESSION:
            {
                #define YAML_BOOLEAN_FMT "%svalue: %s\n"
                const char *value = expr->expr.boolean_expression.value ? "true" : "false";
                bytes_to_write = snprintf(NULL, 0, YAML_BOOLEAN_FMT, padding, value);
                ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                snprintf(&buffer[*buffer_offset], bytes_to_write + 1, YAML_BOOLEAN_FMT, padding, value);
                *buffer_offset += bytes_to_write;
            } break;

            case AST_PREFIX_EXPRESSION:
            {
                { // write op
                    #define EXPRESSION_OP_FMT "%sop: \"%c\"\n"
                    char op = expr->expr.prefix_expression.op;
                    bytes_to_write = snprintf(NULL, 0, EXPRESSION_OP_FMT, padding, op);
                    ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                    snprintf(&buffer[*buffer_offset], bytes_to_write + 1, EXPRESSION_OP_FMT, padding, op);
                    *buffer_offset += bytes_to_write;
                }

                { // write RHS expr
                    #define EXPRESSION_FMT "%sexpr:\n"
                    bytes_to_write = snprintf(NULL, 0, EXPRESSION_FMT, padding);
                    ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                    snprintf(&buffer[*buffer_offset], bytes_to_write + 1, EXPRESSION_FMT, padding);
                    *buffer_offset += bytes_to_write;
                }

                const Expression *nexpr = expr->expr.prefix_expression.rhs;
                ast_print_expression_yaml(nexpr, buffer, buffer_len, buffer_offset, indent_level + 1);
            } break;

            case AST_INFIX_EXPRESSION:
            {
                const Infix_Expression *inexpr = &expr->expr.infix_expression;

                { // write op
                    #define OP_FMT "%sop: \"%s\"\n"
                    const char *op_str = op_to_string(inexpr->op);
                    bytes_to_write = snprintf(NULL, 0, OP_FMT, padding, op_str);
                    snprintf(&buffer[*buffer_offset], bytes_to_write + 1, OP_FMT, padding, op_str);
                    *buffer_offset += bytes_to_write;
                }

                { // write LHS
                    #define LHS_EXPRESSION_FMT "%sLHS:\n"
                    bytes_to_write = snprintf(NULL, 0, LHS_EXPRESSION_FMT, padding);
                    ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                    snprintf(&buffer[*buffer_offset], bytes_to_write + 1, LHS_EXPRESSION_FMT, padding);
                    *buffer_offset += bytes_to_write;
 
                    const Expression *lhs = inexpr->lhs;
                    ast_print_expression_yaml(lhs, buffer, buffer_len, buffer_offset, indent_level + 1);
                }

                { // write RHS
                    #define RHS_EXPRESSION_FMT "%sRHS:\n"
                    bytes_to_write = snprintf(NULL, 0, RHS_EXPRESSION_FMT, padding);
                    ast_print_resize_debug_buffer(buffer, buffer_len, *buffer_offset, bytes_to_write);
                    snprintf(&buffer[*buffer_offset], bytes_to_write + 1, RHS_EXPRESSION_FMT, padding);
                    *buffer_offset += bytes_to_write;

                    const Expression *rhs = inexpr->rhs;
                    ast_print_expression_yaml(rhs, buffer, buffer_len, buffer_offset, indent_level + 1);
                }
            } break;
        }
    }

    free(padding);
}

void ast_print_resize_debug_buffer(
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    size_t buffer_offset,
    int bytes_to_write
)
{
    if ((buffer_offset + bytes_to_write) >= *buffer_len)
    {
        char *new_buffer = realloc(buffer, *buffer_len * 2);
        *buffer_len *= 2;
        if (new_buffer != buffer) 
        {
            buffer = new_buffer;
        }
    }
}

const char *op_to_string(Token_Kind op)
{
    const char *res;
    switch (op)
    {
        case TK_PLUS:     { res = "+"; } break;
        case TK_MINUS:    { res = "-"; } break;
        case TK_ASTERISK: { res = "*"; } break;
        case TK_SLASH:    { res = "/"; } break;
        case TK_LT:       { res = "<"; } break;
        case TK_GT:       { res = ">"; } break;
        case TK_EQ:       { res = "=="; } break;
        case TK_NEQ:      { res = "!="; } break;
        default:          { res = token_kind_to_string(op); } break;
    }
    return res;
}
