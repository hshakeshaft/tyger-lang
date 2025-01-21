#ifndef TYGER_LEXER_INTERNAL_H_
#define TYGER_LEXER_INTERNAL_H_
#include "lexer.h"

void lexer_read_char(Lexer *lexer);
char lexer_peek_char(const Lexer *lexer);
void lexer_skip_whitespace(Lexer *lexer);

#endif // TYGER_LEXER_INTERNAL_H_
