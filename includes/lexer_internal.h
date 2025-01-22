#ifndef TYGER_LEXER_INTERNAL_H_
#define TYGER_LEXER_INTERNAL_H_
#include "tstrings.h"
#include "lexer.h"

void lexer_read_char(Lexer *lexer);
char lexer_peek_char(const Lexer *lexer);
void lexer_skip_whitespace(Lexer *lexer);

void lexer_read_number(Lexer *lexer);
void lexer_read_string(Lexer *lexer);

Token_Kind string_view_to_number_kind(String_View sv);

#endif // TYGER_LEXER_INTERNAL_H_
