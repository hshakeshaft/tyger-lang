#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <stdint.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "trace.h"

#define PARSER_TEST_UTIL_IMPL
#include "parser_test_util.hpp"

// TODO(HS): add parser error checking

TEST(ParserTestSuite, Parse_Var_Statement)
{
    struct Test_Case 
    { 
        const char *input;
        const char *expected_ident;
        Expression expected;
    };

    Expression lhs1{ AST_INT_EXPRESSION, { .int_expression = { 5 } } };
    Expression rhs1_lhs{ AST_INT_EXPRESSION, { .int_expression = { 4 } } };
    Expression rhs1_rhs{ AST_INT_EXPRESSION, { .int_expression = { 3 } } };
    Expression rhs1{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_ASTERISK, &rhs1_lhs, &rhs1_rhs} } };

    Expression rhs2{ AST_INT_EXPRESSION, { .int_expression = { 5 } } };

    std::vector<Test_Case> test_cases{
        { "var x = 10;", "x", Expression{ AST_INT_EXPRESSION, { .int_expression = { 10 } } } },
        { "var fooBar = 10;", "fooBar", Expression{ AST_INT_EXPRESSION, { .int_expression = { 10 } } } },

        { "var PI = 3.14159;", "PI", Expression{ AST_FLOAT_EXPRESSION, { .float_expression = { 3.14159f } } } },

        { "var y = -5;", "y", Expression{ AST_PREFIX_EXPRESSION, { .prefix_expression = { '-', &rhs2 } } } },

        { "var a = b;", "a", Expression{ AST_IDENT_EXPRESSION, { .ident_expression = { "b" } }} },

        { "var x = 5 + 4 * 3;", "x", Expression{ AST_INFIX_EXPRESSION, { .infix_expression { TK_PLUS, &lhs1, &rhs1 } } } },
    };

    for (auto& tc : test_cases)
    {
        Lexer l;
        Parser p;

        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);
        const char *prog_str = program_print_ast(&program, PRINT_FORMAT_YAML);

        EXPECT_EQ(program.len, 1) << prog_str;

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_VAR_STATEMENT)
            << "Expected kind " << ast_statement_kind_to_str(AST_VAR_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind)
            << "\n" << prog_str;
        
        std::string expected_ident{tc.expected_ident};
        std::string actual_ident{stmt.stmt.var_statement.ident};
        EXPECT_EQ(expected_ident, actual_ident) << prog_str;

        Expression expr = stmt.stmt.var_statement.expression;
        test_expression(tc.expected, expr, prog_str);

        program_free(&program);
        free((void *) prog_str);
    }
}

TEST(ParserTestSuite, Parse_Return_Statements)
{
    struct Test_Case 
    { 
        const char *input;
    };

    std::vector<Test_Case> test_cases{
        { "return 10;"    },
        { "return false;" },
        { "return;"       },
        { "return x;"     },
    };

    for (auto& tc : test_cases)
    {
        Lexer l;
        Parser p;

        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);
        const char *prog_str = program_print_ast(&program, PRINT_FORMAT_YAML);

        EXPECT_EQ(program.len, 1) << prog_str;

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_RETURN_STATEMENT)
            << "Expected kind " << ast_statement_kind_to_str(AST_RETURN_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind)
            << "\n" << prog_str;
        
        program_free(&program);
        free((void *) prog_str);
    }
}

TEST(ParserTestSuite, Parse_Ident_Expression)
{
    struct Test_Case
    {
        const char *input;
        const char *ident;
        int expected_ident_len;
    };

    std::vector<Test_Case> test_cases{
        { "x;", "x", 1 },
        { "foo;", "foo", 3 },
        { "foo_bar;", "foo_bar", 7 },
    };

    for (auto &tc : test_cases)
    {
        Lexer l;
        Parser p;

        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);
        const char *prog_str = program_print_ast(&program, PRINT_FORMAT_YAML);

        EXPECT_EQ(program.len, 1) << prog_str;

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind)
            << "\n" << prog_str;

        Expression expr = stmt.stmt.expression_statement.expression;

        EXPECT_EQ(expr.kind, AST_IDENT_EXPRESSION)
            << "Expected kind " << ast_expression_kind_to_str(AST_IDENT_EXPRESSION)
            << ", got " << ast_expression_kind_to_str(expr.kind)
            << "\n" << prog_str;

        Ident_Expression iexpr = expr.expr.ident_expression;
        EXPECT_TRUE(strncmp(iexpr.ident, tc.ident, tc.expected_ident_len) == 0)
            << "Expected ident `" << tc.ident 
            << "`, got `" << iexpr.ident << "`"
            << "\n" << prog_str;

        program_free(&program);
        free((void *) prog_str);
    }
}

TEST(ParserTestSuite, Parse_Int_Expression)
{
    struct Test_Case
    {
        const char *input;
        int32_t expected_value;
    };

    std::vector<Test_Case> test_cases{
        { "10;", 10 },
        { "0;", 0 },
    };

    for (auto &tc : test_cases)
    {
        Lexer l;
        Parser p;

        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);
        const char *prog_str = program_print_ast(&program, PRINT_FORMAT_YAML);

        EXPECT_EQ(program.len, 1) << prog_str;

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind)
            << "\n" << prog_str;

        Expression expr = stmt.stmt.expression_statement.expression;

        EXPECT_EQ(expr.kind, AST_INT_EXPRESSION)
            << "Expected kind " << ast_expression_kind_to_str(AST_INT_EXPRESSION)
            << ", got " << ast_expression_kind_to_str(expr.kind)
            << "\n" << prog_str;

        Int_Expression iexpr = expr.expr.int_expression;
        EXPECT_EQ(tc.expected_value, iexpr.value) << prog_str;

        program_free(&program);
        free((void *) prog_str);
    }
}

TEST(ParserTestSuite, Parse_Float_Expression)
{
    struct Test_Case
    {
        const char *input;
        float expected_value;
    };

    std::vector<Test_Case> test_cases{
        { "10.1;", 10.1f },
        { "3.14159;", 3.14159f },
    };

    for (auto &tc : test_cases)
    {
        Lexer l;
        Parser p;

        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);
        const char *prog_str = program_print_ast(&program, PRINT_FORMAT_YAML);

        EXPECT_EQ(program.len, 1) << prog_str;

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind)
            << "\n" << prog_str;

        Expression expr = stmt.stmt.expression_statement.expression;

        EXPECT_EQ(expr.kind, AST_FLOAT_EXPRESSION)
            << "Expected kind " << ast_expression_kind_to_str(AST_FLOAT_EXPRESSION)
            << ", got " << ast_expression_kind_to_str(expr.kind)
            << "\n" << prog_str;

        Float_Expression fexpr = expr.expr.float_expression;
        EXPECT_FLOAT_EQ(tc.expected_value, fexpr.value) << prog_str;

        program_free(&program);
        free((void *) prog_str);
    }
}

TEST(ParserTestSuite, Parse_Boolean_Expression)
{
    struct Test_Case
    {
        const char *input;
        Expression expected;
    };

    std::vector<Test_Case> test_cases{
        { "false;",  Expression{ AST_BOOLEAN_EXPRESSION, { .boolean_expression = { false } } } },
        { "true;",   Expression{ AST_BOOLEAN_EXPRESSION, { .boolean_expression = { true } } } },
    };

    for (auto& tc : test_cases)
    {
        Lexer l;
        Parser p;

        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);
        const char *prog_str = program_print_ast(&program, PRINT_FORMAT_YAML);

        EXPECT_EQ(program.len, 1) << prog_str;

        Statement stmt = program.statements[0];
        
        Expression act = stmt.stmt.expression_statement.expression;
        test_expression(tc.expected, act, prog_str);

        program_free(&program);
        free((void*) prog_str);
    }
}

// TODO(HS): decide if I want to allow `!<number>` in parser
// TODO(HS): ^above for strings too
TEST(ParserTestSuite, Parse_Prefix_Expression)
{
    struct Test_Case
    {
        const char *input;
        Expression expected;
    };

    Expression expr1 = Expression{ AST_INT_EXPRESSION, { .int_expression = { 10 } } };
    Expression expr2 = Expression{ AST_FLOAT_EXPRESSION, { .float_expression = { 3.1f } } };
    Expression expr3 = Expression{ AST_BOOLEAN_EXPRESSION, { .boolean_expression = { false } } };
    Expression expr4 = Expression{ AST_BOOLEAN_EXPRESSION, { .boolean_expression = { true } } };

    std::vector<Test_Case> test_cases{
        { "-10;",    Expression{ AST_PREFIX_EXPRESSION, { .prefix_expression = { '-', &expr1 } } } },
        { "-3.1;",   Expression{ AST_PREFIX_EXPRESSION, { .prefix_expression = { '-', &expr2 } } } },
        { "!false;", Expression{ AST_PREFIX_EXPRESSION, { .prefix_expression = { '!', &expr3 } } } },
        { "!true;",  Expression{ AST_PREFIX_EXPRESSION, { .prefix_expression = { '!', &expr4 } } } },
    };

    for (auto &tc : test_cases)
    {
        Lexer l;
        Parser p;

        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);
        const char *prog_str = program_print_ast(&program, PRINT_FORMAT_YAML);

        EXPECT_EQ(program.len, 1) << prog_str;

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind)
            << "\n" << prog_str;

        Expression expr = stmt.stmt.expression_statement.expression;

        test_expression(tc.expected, expr, prog_str);

        program_free(&program);
        free((void *) prog_str);
    }
}

TEST(ParserTestSuite, Parse_Binary_Expression)
{
    struct Test_Case
    {
        const char *input;
        Token_Kind expected_op;
        Expression lhs;
        Expression rhs;
    };

    std::vector<Test_Case> test_cases{
        { "5 + 5;",  TK_PLUS,     Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
        { "5 - 5;",  TK_MINUS,    Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
        { "5 * 5;",  TK_ASTERISK, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
        { "5 / 5;",  TK_SLASH,    Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
        { "5 < 5;",  TK_LT,       Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
        { "5 > 5;",  TK_GT,       Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
        { "5 == 5;", TK_EQ,       Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
        { "5 != 5;", TK_NEQ,      Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
    };

    for (auto& tc : test_cases)
    {
        Lexer l;
        Parser p;
        
        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);
        const char *prog_str = program_print_ast(&program, PRINT_FORMAT_YAML);

        EXPECT_EQ(program.len, 1) << prog_str;

        Statement stmt = program.statements[0];
        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind)
            << "\n" << prog_str;

        Expression expr = stmt.stmt.expression_statement.expression;
        EXPECT_EQ(expr.kind, AST_INFIX_EXPRESSION)
            << "Expected kind " << ast_expression_kind_to_str(AST_INFIX_EXPRESSION)
            << ", got " << ast_expression_kind_to_str(expr.kind)
            << "\n" << prog_str;
        
        Infix_Expression inexpr = expr.expr.infix_expression;

        // test operator
        EXPECT_EQ(inexpr.op, tc.expected_op)
            << "Expected operator " << token_kind_to_string(tc.expected_op)
            << ", got " << token_kind_to_string(inexpr.op)
            << "\n" << prog_str;

        // test LHS
        Expression *lhs = inexpr.lhs;
        EXPECT_EQ(lhs->kind, tc.lhs.kind)
            << "Expected kind " << ast_expression_kind_to_str(tc.lhs.kind)
            << ", got " << ast_expression_kind_to_str(lhs->kind)
            << "\n" << prog_str;

        // test RHS
        Expression *rhs = inexpr.rhs;
        EXPECT_EQ(rhs->kind, tc.rhs.kind)
            << "Expected kind " << ast_expression_kind_to_str(tc.rhs.kind)
            << ", got " << ast_expression_kind_to_str(rhs->kind)
            << "\n" << prog_str;

        program_free(&program);
        free((void *) prog_str);
    }
}

TEST(ParserTestSuite, Parse_Operator_Precidence)
{
    struct Test_Case
    {
        const char *input;
        // debug print out of the AST in `plain` format
        const char *expected_ast;
        size_t expected_len;
    };

    std::vector<Test_Case> test_cases{
        { "5 + 4;",     "(5 + 4)"      , 1 },
        { "5 + 4 + 3;", "((5 + 4) + 3)", 1 },
        { "5 - 4 - 3;", "((5 - 4) - 3)", 1 },
        { "5 + 4 - 3;", "((5 + 4) - 3)", 1 },
        { "5 * 4 / 3;", "((5 * 4) / 3)", 1 },

        { "5 > 4 == 3 < 4;", "((5 > 4) == (3 < 4))", 1 },
        { "5 > 4 != 3 < 4;", "((5 > 4) != (3 < 4))", 1 },

        { "3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))", 1 },

        { "-a * b;",    "((-a) * b)",    1 },
        { "a - b + c;", "((a - b) + c)", 1 },
        { "a + b * c;", "(a + (b * c))", 1 },
        { "a * b * c;", "((a * b) * c)", 1 },
        { "a + b / c;", "(a + (b / c))", 1 },

        { "a + b * c + d / e - f;", "(((a + (b * c)) + (d / e)) - f)", 1 },

        { "a + b; -c * d;", "(a + b)\n((-c) * d)", 2 },
    };

    for (auto& tc : test_cases)
    {
        Lexer l;
        Parser p;
        
        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);
        const char *prog_str = program_print_ast(&program, PRINT_FORMAT_PLAIN);
        const char *prog_yml = program_print_ast(&program, PRINT_FORMAT_YAML);

        EXPECT_EQ(program.len, tc.expected_len) << "\n" << prog_str << "\n" << prog_yml;

        for (size_t i = 0; i < program.len; ++i)
        {
            Statement stmt = program.statements[i];

            EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT)
                << "Expected statement kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
                << ", got " << ast_statement_kind_to_str(stmt.kind)
                << "\n" << prog_yml;
        }

        std::string act_ast{prog_str};
        std::string exp_ast{tc.expected_ast};
        EXPECT_EQ(act_ast, exp_ast) << "\n" << prog_yml;

        program_free(&program);
        free((void *) prog_str);
        free((void *) prog_yml);
    }
}
