#ifndef TYGER_TRACE_INTERNAL_H_
#define TYGER_TRACE_INTERNAL_H_
#include "parser.h"

// TODO(HS): better document WTF this actually means
// NOTE(HS): function pointers are declared as types for later dynamic dispatch
typedef void (*Print_Header_Fn) (char *, size_t *, size_t *);
typedef void (*Print_Statement_Fn)  (const Statement *, char *, size_t *, size_t *);

/// 
/// Plain debug print functions
///

// NOTE(HS): functionally a "no-op" (not really, but doesn't mutate inputs)
void ast_print_header_plain(
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
);
void ast_print_statement_plain(
    const Statement *stmt,
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
);
void ast_print_expression_plain(
    const Expression *expr,
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
);


/// 
/// YAML debug print functions
///

void ast_print_header_yaml(
    INOUT char *buffer,
    INOUT size_t *buffer_len,
    INOUT size_t *buffer_offset
);
void ast_print_statement_yaml(
    const Statement *stmt,
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

///
/// Util functions
///

// resize the buffer the debug string is written to if writing chars would write
// beyond the end of buffer.
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
