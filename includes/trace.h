/**
 * Debug functions to print the generate a formatted string representation of the
 * AST in defined formats.
*/
#ifndef TYGER_TRACE_H_
#define TYGER_TRACE_H_
#include "parser.h"

/// Defines the method to use to print the AST of the program as.
typedef enum
{
    ///  print AST in paren delimited format, i.e. `5 + 4 + 3` => `((5 + 4) + 3)`
    PRINT_FORMAT_PLAIN,
    /// print AST in YAML format
    PRINT_FORMAT_YAML,
} AST_Print_Format;

#if defined(__cplusplus)
extern "C" {
#endif

/// Returns pointer to heap allocated string representing the AST in specified format
const char *program_print_ast(const Program *prog, AST_Print_Format format);

#if defined(__cplusplus)
}
#endif

#endif // TYGER_TRACE_H_
