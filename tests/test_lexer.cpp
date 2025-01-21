#include <gtest/gtest.h>
#include <vector>
#include <stdio.h>

#include "tstrings.h"
#include "lexer.h"

const char *prog =
    "+ - * / ! =\n"
    "!= == < > <= >= || &&\n"
    "()[]{}\n"
    ":;,.\n"
    "\n"
    "0 1 100 1000 -10\n"
    "0.0 3.1415 148.012 -0.8123\n"
    "\"foo\" \"bar\" \"Hello, World!\" \"this is \\\"escaped\\\"\"\n"
    "true false\n"
    "nil\n"
    "\n"
    "if else var func return\n"
    "println\n"
    "\n"
    "\n"
    "var x = 10;\n"
    "var PI = 3.14;\n"
    "var snake_case_var = 105;\n"
    "\n"
    "5 + 4 - 3 * 2 / 1;\n"
    "\n"
    "if (true) {\n"
    "    x = x * 2;\n"
    "} else {\n"
    "    x = x / 2;\n"
    "}\n"
    "\n"
    "var sqaure = func(n) {\n"
    "    return n * n;\n"
    "};\n"
    "\n"
    "println(\"Hello, World!\");\n"
    "\n";

auto expected_tokens = std::vector<Token>{
    Token{ TK_PLUS, { 0, 1, 1 }, { (char*) &prog[0], 1 } },
    Token{ TK_MINUS, { 2, 1, 3 }, { (char*) &prog[2], 1 } },
    Token{ TK_ASTERISK, { 4, 1, 5 }, { (char*) &prog[4], 1 } },
    Token{ TK_SLASH, { 6, 1, 7 }, { (char*) &prog[6], 1 } },
    Token{ TK_BANG, { 8, 1, 9 }, { (char*) &prog[8], 1 } },
    Token{ TK_ASSIGN, { 10, 1, 11 }, { (char*) &prog[10], 1 } },
    Token{ TK_NEQ, { 12, 2, 1 }, { (char*) &prog[12], 2 } },
    Token{ TK_EQ, { 15, 2, 4 }, { (char*) &prog[15], 2 } },
    Token{ TK_LT, { 18, 2, 7 }, { (char*) &prog[18], 1 } },
    Token{ TK_GT, { 20, 2, 9 }, { (char*) &prog[20], 1 } },
    Token{ TK_LTE, { 22, 2, 11 }, { (char*) &prog[22], 2 } },
    Token{ TK_GTE, { 25, 2, 14 }, { (char*) &prog[25], 2 } },
    Token{ TK_LOR, { 28, 2, 17 }, { (char*) &prog[28], 2 } },
    Token{ TK_LAND, { 31, 2, 20 }, { (char*) &prog[31], 2 } },
    Token{ TK_LPAREN, { 34, 3, 1 }, { (char*) &prog[34], 1 } },
    Token{ TK_RPAREN, { 35, 3, 2 }, { (char*) &prog[35], 1 } },
    Token{ TK_LBRACKET, { 36, 3, 3 }, { (char*) &prog[36], 1 } },
    Token{ TK_RBRACKET, { 37, 3, 4 }, { (char*) &prog[37], 1 } },
    Token{ TK_LBRACE, { 38, 3, 5 }, { (char*) &prog[38], 1 } },
    Token{ TK_RBRACE, { 39, 3, 6 }, { (char*) &prog[39], 1 } },
    Token{ TK_COLON, { 41, 4, 1 }, { (char*) &prog[41], 1 } },
    Token{ TK_SEMICOLON, { 42, 4, 2 }, { (char*) &prog[42], 1 } },
    Token{ TK_COMMA, { 43, 4, 3 }, { (char*) &prog[43], 1 } },
    Token{ TK_PERIOD, { 44, 4, 4 }, { (char*) &prog[44], 1 } },
    Token{ TK_INT_LIT, { 47, 6, 1 }, { (char*) &prog[47], 1 } },
    Token{ TK_INT_LIT, { 49, 6, 3 }, { (char*) &prog[49], 1 } },
    Token{ TK_INT_LIT, { 51, 6, 5 }, { (char*) &prog[51], 3 } },
    Token{ TK_INT_LIT, { 55, 6, 9 }, { (char*) &prog[55], 4 } },
    Token{ TK_MINUS, { 60, 6, 14 }, { (char*) &prog[60], 1 } },
    Token{ TK_INT_LIT, { 61, 6, 15 }, { (char*) &prog[61], 2 } },
    Token{ TK_FLOAT_LIT, { 64, 7, 1 }, { (char*) &prog[64], 3 } },
    Token{ TK_FLOAT_LIT, { 68, 7, 5 }, { (char*) &prog[68], 6 } },
    Token{ TK_FLOAT_LIT, { 75, 7, 12 }, { (char*) &prog[75], 7 } },
    Token{ TK_MINUS, { 83, 7, 20 }, { (char*) &prog[83], 1 } },
    Token{ TK_FLOAT_LIT, { 84, 7, 21 }, { (char*) &prog[84], 6 } },
    Token{ TK_STRING_LIT, { 92, 8, 2 }, { (char*) &prog[92], 3 } },
    Token{ TK_STRING_LIT, { 98, 8, 8 }, { (char*) &prog[98], 3 } },
    Token{ TK_STRING_LIT, { 104, 8, 14 }, { (char*) &prog[104], 13 } },
    Token{ TK_STRING_LIT, { 120, 8, 30 }, { (char*) &prog[120], 19 } },
    Token{ TK_TRUE, { 141, 9, 1 }, { (char*) &prog[141], 4 } },
    Token{ TK_FALSE, { 146, 9, 6 }, { (char*) &prog[146], 5 } },
    Token{ TK_NIL, { 152, 10, 1 }, { (char*) &prog[152], 3 } },
    Token{ TK_IF, { 157, 12, 1 }, { (char*) &prog[157], 2 } },
    Token{ TK_ELSE, { 160, 12, 4 }, { (char*) &prog[160], 4 } },
    Token{ TK_VAR, { 165, 12, 9 }, { (char*) &prog[165], 3 } },
    Token{ TK_FUNC, { 169, 12, 13 }, { (char*) &prog[169], 4 } },
    Token{ TK_RETURN, { 174, 12, 18 }, { (char*) &prog[174], 6 } },
    Token{ TK_PRINTLN, { 181, 13, 1 }, { (char*) &prog[181], 7 } },
    Token{ TK_VAR, { 191, 16, 1 }, { (char*) &prog[191], 3 } },
    Token{ TK_IDENT, { 195, 16, 5 }, { (char*) &prog[195], 1 } },
    Token{ TK_ASSIGN, { 197, 16, 7 }, { (char*) &prog[197], 1 } },
    Token{ TK_INT_LIT, { 199, 16, 9 }, { (char*) &prog[199], 2 } },
    Token{ TK_SEMICOLON, { 201, 16, 11 }, { (char*) &prog[201], 1 } },
    Token{ TK_VAR, { 203, 17, 1 }, { (char*) &prog[203], 3 } },
    Token{ TK_IDENT, { 207, 17, 5 }, { (char*) &prog[207], 2 } },
    Token{ TK_ASSIGN, { 210, 17, 8 }, { (char*) &prog[210], 1 } },
    Token{ TK_FLOAT_LIT, { 212, 17, 10 }, { (char*) &prog[212], 4 } },
    Token{ TK_SEMICOLON, { 216, 17, 14 }, { (char*) &prog[216], 1 } },
    Token{ TK_VAR, { 218, 18, 1 }, { (char*) &prog[218], 3 } },
    Token{ TK_IDENT, { 222, 18, 5 }, { (char*) &prog[222], 14 } },
    Token{ TK_ASSIGN, { 237, 18, 20 }, { (char*) &prog[237], 1 } },
    Token{ TK_INT_LIT, { 239, 18, 22 }, { (char*) &prog[239], 3 } },
    Token{ TK_SEMICOLON, { 242, 18, 25 }, { (char*) &prog[242], 1 } },
    Token{ TK_INT_LIT, { 245, 20, 1 }, { (char*) &prog[245], 1 } },
    Token{ TK_PLUS, { 247, 20, 3 }, { (char*) &prog[247], 1 } },
    Token{ TK_INT_LIT, { 249, 20, 5 }, { (char*) &prog[249], 1 } },
    Token{ TK_MINUS, { 251, 20, 7 }, { (char*) &prog[251], 1 } },
    Token{ TK_INT_LIT, { 253, 20, 9 }, { (char*) &prog[253], 1 } },
    Token{ TK_ASTERISK, { 255, 20, 11 }, { (char*) &prog[255], 1 } },
    Token{ TK_INT_LIT, { 257, 20, 13 }, { (char*) &prog[257], 1 } },
    Token{ TK_SLASH, { 259, 20, 15 }, { (char*) &prog[259], 1 } },
    Token{ TK_INT_LIT, { 261, 20, 17 }, { (char*) &prog[261], 1 } },
    Token{ TK_SEMICOLON, { 262, 20, 18 }, { (char*) &prog[262], 1 } },
    Token{ TK_IF, { 265, 22, 1 }, { (char*) &prog[265], 2 } },
    Token{ TK_LPAREN, { 268, 22, 4 }, { (char*) &prog[268], 1 } },
    Token{ TK_TRUE, { 269, 22, 5 }, { (char*) &prog[269], 4 } },
    Token{ TK_RPAREN, { 273, 22, 9 }, { (char*) &prog[273], 1 } },
    Token{ TK_LBRACE, { 275, 22, 11 }, { (char*) &prog[275], 1 } },
    Token{ TK_IDENT, { 281, 23, 5 }, { (char*) &prog[281], 1 } },
    Token{ TK_ASSIGN, { 283, 23, 7 }, { (char*) &prog[283], 1 } },
    Token{ TK_IDENT, { 285, 23, 9 }, { (char*) &prog[285], 1 } },
    Token{ TK_ASTERISK, { 287, 23, 11 }, { (char*) &prog[287], 1 } },
    Token{ TK_INT_LIT, { 289, 23, 13 }, { (char*) &prog[289], 1 } },
    Token{ TK_SEMICOLON, { 290, 23, 14 }, { (char*) &prog[290], 1 } },
    Token{ TK_RBRACE, { 292, 24, 1 }, { (char*) &prog[292], 1 } },
    Token{ TK_ELSE, { 294, 24, 3 }, { (char*) &prog[294], 4 } },
    Token{ TK_LBRACE, { 299, 24, 8 }, { (char*) &prog[299], 1 } },
    Token{ TK_IDENT, { 305, 25, 5 }, { (char*) &prog[305], 1 } },
    Token{ TK_ASSIGN, { 307, 25, 7 }, { (char*) &prog[307], 1 } },
    Token{ TK_IDENT, { 309, 25, 9 }, { (char*) &prog[309], 1 } },
    Token{ TK_SLASH, { 311, 25, 11 }, { (char*) &prog[311], 1 } },
    Token{ TK_INT_LIT, { 313, 25, 13 }, { (char*) &prog[313], 1 } },
    Token{ TK_SEMICOLON, { 314, 25, 14 }, { (char*) &prog[314], 1 } },
    Token{ TK_RBRACE, { 316, 26, 1 }, { (char*) &prog[316], 1 } },
    Token{ TK_VAR, { 319, 28, 1 }, { (char*) &prog[319], 3 } },
    Token{ TK_IDENT, { 323, 28, 5 }, { (char*) &prog[323], 6 } },
    Token{ TK_ASSIGN, { 330, 28, 12 }, { (char*) &prog[330], 1 } },
    Token{ TK_FUNC, { 332, 28, 14 }, { (char*) &prog[332], 4 } },
    Token{ TK_LPAREN, { 336, 28, 18 }, { (char*) &prog[336], 1 } },
    Token{ TK_IDENT, { 337, 28, 19 }, { (char*) &prog[337], 1 } },
    Token{ TK_RPAREN, { 338, 28, 20 }, { (char*) &prog[338], 1 } },
    Token{ TK_LBRACE, { 340, 28, 22 }, { (char*) &prog[340], 1 } },
    Token{ TK_RETURN, { 346, 29, 5 }, { (char*) &prog[346], 6 } },
    Token{ TK_IDENT, { 353, 29, 12 }, { (char*) &prog[353], 1 } },
    Token{ TK_ASTERISK, { 355, 29, 14 }, { (char*) &prog[355], 1 } },
    Token{ TK_IDENT, { 357, 29, 16 }, { (char*) &prog[357], 1 } },
    Token{ TK_SEMICOLON, { 358, 29, 17 }, { (char*) &prog[358], 1 } },
    Token{ TK_RBRACE, { 360, 30, 1 }, { (char*) &prog[360], 1 } },
    Token{ TK_SEMICOLON, { 361, 30, 2 }, { (char*) &prog[361], 1 } },
    Token{ TK_PRINTLN, { 364, 32, 1 }, { (char*) &prog[364], 7 } },
    Token{ TK_LPAREN, { 371, 32, 8 }, { (char*) &prog[371], 1 } },
    Token{ TK_STRING_LIT, { 373, 32, 10 }, { (char*) &prog[373], 13 } },
    Token{ TK_RPAREN, { 387, 32, 24 }, { (char*) &prog[387], 1 } },
    Token{ TK_SEMICOLON, { 388, 32, 25 }, { (char*) &prog[388], 1 } },
    Token{ TK_EOF, { 390, 32, 26 }, { (char*) &prog[390], 1 } },
};

TEST(LexerTestSuite, test_lexer)
{
    Lexer l;
    lexer_init(&l, prog);

    Token actual;
    size_t expected_index = 0;
    do
    {
        auto& expected = expected_tokens[expected_index];
        actual = lexer_next_token(&l);

        // assert correct kind of token lexed
        ASSERT_EQ(expected.kind, actual.kind)
            << "Expected token to have kind " << token_kind_to_string(expected.kind)
            << ", got " << token_kind_to_string(actual.kind)
            << std::endl;

        // assert read from correct location
        ASSERT_EQ(expected.location.pos,  actual.location.pos);
        ASSERT_EQ(expected.location.line, actual.location.line);
        ASSERT_EQ(expected.location.col,  actual.location.col);

        // assert literals are the same
        // TODO(HS): check literals pointed to are equal
        char expected_buffer[64];
        snprintf(expected_buffer, sizeof(expected_buffer), sv_fmt, sv_args(expected.literal));

        char actual_buffer[64];
        snprintf(actual_buffer, sizeof(actual_buffer), sv_fmt, sv_args(actual.literal));

        ASSERT_TRUE(string_view_eq(expected.literal, actual.literal))
            << "Expected Literal \"" << expected_buffer << "\", got \""
            << actual_buffer << "\"";

        expected_index += 1;
    } while (actual.kind != TK_EOF && expected_index < expected_tokens.size());

    ASSERT_EQ((expected_index + 1), expected_tokens.size());
}
