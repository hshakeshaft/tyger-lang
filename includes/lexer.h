#ifndef TYGER_LEXER_H_
#define TYGER_LEXER_H_
#include <stddef.h>
#include "tstrings.h"

typedef enum
{
    TK_EOF,
    TK_ILLEGAL,
    TK_PLUS,
    TK_MINUS,
    TK_ASTERISK,
    TK_SLASH,
    TK_ASSIGN,
    TK_BANG,
    TK_EQ,
    TK_NEQ,
    TK_LT,
    TK_GT,
    TK_LTE,
    TK_GTE,
    TK_LOR,
    TK_LAND,
    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACE,
    TK_RBRACE,
    TK_LBRACKET,
    TK_RBRACKET,
    TK_SEMICOLON,
    TK_COLON,
    TK_COMMA,
    TK_PERIOD,
    TK_IDENT,
    TK_INT_LIT,
    TK_FLOAT_LIT,
    TK_STRING_LIT,
    TK_VAR,
    TK_IF,
    TK_ELSE,
    TK_FUNC,
    TK_RETURN,
    TK_TRUE,
    TK_FALSE,
    TK_NIL,
    TK_PRINTLN,
    TOKEN_KIND_COUNT
} Token_Kind;

typedef struct
{
    size_t pos;
    size_t line;
    size_t col;
} Location;

typedef struct
{
    Token_Kind kind;
    Location location;
    String_View literal;
}Token;

typedef struct
{
    const char *input;
    size_t input_len;
    Location location;
    size_t read_pos;
    char ch;
} Lexer;

#if defined(__cplusplus)
extern "C" {
#endif

const char *token_kind_to_string(Token_Kind kind);

void lexer_init(Lexer *lexer, const char *input);
Token lexer_next_token(Lexer *lexer);

#if defined(__cplusplus)
}
#endif

#endif // TYGER_LEXER_H_
