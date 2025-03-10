#ifndef TYGER_TRACE_INTERNAL_H_
#define TYGER_TRACE_INTERNAL_H_
#include "parser.h"

// TODO(HS): better document WTF this actually means
// NOTE(HS): function pointers are declared as types for later dynamic dispatch
typedef void (*Print_Header_Fn) (char *, size_t *, size_t *);
typedef void (*Print_Expression_Fn) (const Expression *, char *, size_t *, size_t *, int);
typedef void (*Print_Statement_Fn)  (const Statement *, Print_Expression_Fn, char *, size_t *, size_t *);

// TODO(HS): add "plain" format print functions

void ast_print_header_yaml(
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
);
void ast_print_statement_yaml(
    const Statement *stmt,
    const Print_Expression_Fn print_expression,
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
);
void ast_print_expression_yaml(
    const Expression *expr,
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset,
    int indent_level
);

void ast_print_resize_debug_buffer(
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    size_t buffer_offset,
    int bytes_to_write
);

/// converts an operator to a pretty string version
/// NOTE(HS): will return stringified version of token if operator is invalid
const char *op_to_string(Token_Kind op);

#endif // TYGER_TRACE_INTERNAL_H_
