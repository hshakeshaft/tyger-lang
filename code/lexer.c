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

    switch (lexer->ch)
    {
        br_case('\0', {
            token.kind = TK_EOF;
            token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
        });

        br_case('+', {
            token.kind = TK_PLUS;
            token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
        });
        
        br_case('-', {
            token.kind = TK_MINUS;
            token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
        });

        br_case('*', {
            token.kind = TK_ASTERISK;
            token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
        });

        br_case('/', {
            token.kind = TK_SLASH;
            token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
        });

        br_case('!', {
            if (lexer_peek_char(lexer) == '=')
            {
                token.kind = TK_NEQ;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 2);
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_BANG;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
            }
        });

        br_case('=', {
            if (lexer_peek_char(lexer) == '=')
            {
                token.kind = TK_EQ;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 2);
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_ASSIGN;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
            }
        });

        br_case('<', {
            if (lexer_peek_char(lexer) == '=')
            {
                token.kind = TK_LTE;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 2);
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_LT;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
            }
        });

        br_case('>', {
            if (lexer_peek_char(lexer) == '=')
            {
                token.kind = TK_GTE;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 2);
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_GT;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
            }
        });

        br_case('|', {
            if (lexer_peek_char(lexer) == '|')
            {
                token.kind = TK_LOR;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 2);
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_ILLEGAL;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
            }
        });

        br_case('&', {
            if (lexer_peek_char(lexer) == '&')
            {
                token.kind = TK_LAND;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 2);
                lexer_read_char(lexer);
            }
            else
            {
                token.kind = TK_ILLEGAL;
                token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
            }
        });

        default:
        {
            token.kind = TK_ILLEGAL;
            token.literal = string_view_from_cstr_offset(lexer->input, lexer->location.pos, 1);
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
