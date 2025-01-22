#include <assert.h>
#include <string.h>

#include "lexer.h"
#include "lexer_internal.h"

const char *token_kind_to_string(Token_Kind kind)
{
    // TODO(HS): move this to codegen
    // TODO(HS): force codegen to add the final "count" case
    static const char *str_map[TOKEN_KIND_COUNT] = {
        [TK_EOF] =        "EOF",
        [TK_ILLEGAL] =    "ILLEGAL",
        [TK_PLUS] =       "PLUS",
        [TK_MINUS] =      "MINUS",
        [TK_ASTERISK] =   "ASTERISK",
        [TK_SLASH] =      "SLASH",
        [TK_ASSIGN] =     "ASSIGN",
        [TK_BANG] =       "BANG",
        [TK_EQ] =         "EQ",
        [TK_NEQ] =        "NEQ",
        [TK_LT] =         "LT",
        [TK_GT] =         "GT",
        [TK_LTE] =        "LTE",
        [TK_GTE] =        "GTE",
        [TK_LOR] =        "LOR",
        [TK_LAND] =       "LAND",
        [TK_LPAREN] =     "LPAREN",
        [TK_RPAREN] =     "RPAREN",
        [TK_LBRACE] =     "LBRACE",
        [TK_RBRACE] =     "RBRACE",
        [TK_LBRACKET] =   "LBRACKET",
        [TK_RBRACKET] =   "RBRACKET",
        [TK_SEMICOLON] =  "SEMICOLON",
        [TK_COLON] =      "COLON",
        [TK_COMMA] =      "COMMA",
        [TK_PERIOD] =     "PERIOD",
        [TK_IDENT] =      "IDENT",
        [TK_INT_LIT] =    "INT_LIT",
        [TK_FLOAT_LIT] =  "FLOAT_LIT",
        [TK_STRING_LIT] = "STRING_LIT",
        [TK_VAR] =        "VAR",
        [TK_IF] =         "IF",
        [TK_ELSE] =       "ELSE",
        [TK_FUNC] =       "FUNC",
        [TK_RETURN] =     "RETURN",
        [TK_TRUE] =       "TRUE",
        [TK_FALSE] =      "FALSE",
        [TK_NIL] =        "NIL",
        [TK_PRINTLN] =    "PRINTLN",
    };
    return str_map[kind];
}

void lexer_init(Lexer *lexer, const char *input)
{
    assert(lexer);
    assert(input);

    size_t input_len = strlen(input);
    *lexer = (Lexer) {
        .input = input,
        .input_len = input_len,
        .location = (Location) {
            .pos = 0,
            .line = 1,
            .col = 0,
        },
        .read_pos = 0,
        .ch = '\0',
    };

    lexer_read_char(lexer);

    return;
}

Token lexer_next_token(Lexer *lexer)
{
    lexer_skip_whitespace(lexer);

    Token token = {0};
    token.location = lexer->location;

    // NOTE(HS): this is dumb - but "fallthrough" by default is dump too.
    #define br_case(CASE, ...) \
        case (CASE):           \
        {                      \
            __VA_ARGS__        \
        } break

    // NOTE(HS): default for most characters is to generate something of length 1,
    // this can simply be updated after initial allocation if required, e.g. for
    // `!=`, `==`, and `<=`.
    token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);

    switch (lexer->ch)
    {
        br_case('\0', {
            token.kind = TK_EOF;
        });

        br_case('(', {
            token.kind = TK_LPAREN;
        });

        br_case(')', {
            token.kind = TK_RPAREN;
        });

        br_case('{', {
            token.kind = TK_LBRACE;
        });

        br_case('}', {
            token.kind = TK_RBRACE;
        });

        br_case('[', {
            token.kind = TK_LBRACKET;
        });

        br_case(']', {
            token.kind = TK_RBRACKET;
        });

        br_case(':', {
            token.kind = TK_COLON;
        });

        br_case(';', {
            token.kind = TK_SEMICOLON;
        });

        br_case(',', {
            token.kind = TK_COMMA;
        });

        br_case('.', {
            token.kind = TK_PERIOD;
        });

        br_case('+', {
            token.kind = TK_PLUS;
        });
        
        br_case('-', {
            token.kind = TK_MINUS;
        });

        br_case('*', {
            token.kind = TK_ASTERISK;
        });

        br_case('/', {
            token.kind = TK_SLASH;
        });

        br_case('!', {
            if (lexer_peek_char(lexer) == '=')
            {
                token.kind = TK_NEQ;
                token.literal.length = 2;
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_BANG;
            }
        });

        br_case('=', {
            if (lexer_peek_char(lexer) == '=')
            {
                token.kind = TK_EQ;
                token.literal.length = 2;
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_ASSIGN;
            }
        });

        br_case('<', {
            if (lexer_peek_char(lexer) == '=')
            {
                token.kind = TK_LTE;
                token.literal.length = 2;
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_LT;
            }
        });

        br_case('>', {
            if (lexer_peek_char(lexer) == '=')
            {
                token.kind = TK_GTE;
                token.literal.length = 2;
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_GT;
            }
        });

        br_case('|', {
            if (lexer_peek_char(lexer) == '|')
            {
                token.kind = TK_LOR;
                token.literal.length = 2;
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_ILLEGAL;
            }
        });

        br_case('&', {
            if (lexer_peek_char(lexer) == '&')
            {
                token.kind = TK_LAND;
                token.literal.length = 2;
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_ILLEGAL;
            }
        });

        br_case('\"', {
            lexer_read_char(lexer);
            size_t pos = lexer->location.pos;
            lexer_read_string(lexer);
            size_t len = lexer->location.pos - pos;
            token.literal = string_view_from_cstr_offset(lexer->input, pos, len);
            token.kind = TK_STRING_LIT;
        });

        default:
        {
            if (is_numeric(lexer->ch))
            {
                size_t pos = lexer->location.pos;
                lexer_read_number(lexer);
                token.literal.length = lexer->location.pos - pos;
                token.kind = string_view_to_number_kind(token.literal);
                return token;
            }
            else
            {
                token.kind = TK_ILLEGAL;
            }
        } break;
    }

    lexer_read_char(lexer);
    return token;
}

void lexer_read_char(Lexer *lexer)
{
    assert(lexer);

    if (lexer->read_pos >= lexer->input_len)
    {
        lexer->ch = '\0';
    }
    else
    {
        if (lexer->ch == '\n')
        {
            lexer->location.line += 1;
            lexer->location.col = 1;
        }
        else if (lexer->ch == '\r')
        {
            if (lexer_peek_char(lexer) == '\n')
            {
                lexer->read_pos += 1;
            }
            lexer->location.line += 1;
            lexer->location.col = 1;
        }
        else
        {
            lexer->location.col += 1;
        }
        lexer->ch = lexer->input[lexer->read_pos];
    }

    lexer->location.pos = lexer->read_pos++;
}

inline char lexer_peek_char(const Lexer *lexer)
{
    if (lexer->read_pos >= lexer->input_len)
    {
        return '\0';
    }
    else
    {
        return lexer->input[lexer->read_pos];
    }
}

void lexer_skip_whitespace(Lexer *lexer)
{
    while (is_whitespace(lexer->ch))
    {
        lexer_read_char(lexer);
    }
}

void lexer_read_number(Lexer *lexer)
{
    while ((is_numeric(lexer->ch) || lexer->ch == '.') && !is_end_of_input(lexer->ch))
    {
        lexer_read_char(lexer);
    }
}

void lexer_read_string(Lexer *lexer)
{
    while (lexer->ch != '\"' && !is_end_of_input(lexer->ch))
    {
        if (lexer->ch == '\\')
        {
            if (lexer_peek_char(lexer) == '\"')
            {
                lexer_read_char(lexer);
                lexer_read_char(lexer);
            }
        }
        else
        {
            lexer_read_char(lexer);
        }
    }
}

Token_Kind string_view_to_number_kind(String_View sv)
{
    int decimal_point_count = 0;
    Token_Kind kind = TK_ILLEGAL;

    for (size_t i = 0; i < sv.length; ++i)
    {
        if (sv.str[i] == '.')
        {
            decimal_point_count += 1;
        }
    }

    if (decimal_point_count == 0)
    {
        kind = TK_INT_LIT;
    }
    else if (decimal_point_count == 1)
    {
        kind = TK_FLOAT_LIT;
    }

    return kind;
}
