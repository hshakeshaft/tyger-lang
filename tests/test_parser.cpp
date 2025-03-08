#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <stdint.h>

#include "lexer.h"
#include "parser.h"
#include "ast.h"

// TODO(HS): add parser error checking

TEST(ParserTestSuite, Parse_Var_Statement)
{
    struct Test_Case 
    { 
        const char *input;
        Statement_Kind expected_kind;
        const char *expected_ident;
    };

    std::vector<Test_Case> test_cases{
        { "var x = 10;",       AST_VAR_STATEMENT, "x" },
        { "var y = 10;",       AST_VAR_STATEMENT, "y" },
        { "var fooBar = 10;",  AST_VAR_STATEMENT, "fooBar" },
        { "var PI = 3.14159;", AST_VAR_STATEMENT, "PI" },
    };

    for (auto& tc : test_cases)
    {
        Lexer l;
        Parser p;

        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);

        EXPECT_EQ(program.len, 1);

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_VAR_STATEMENT)
            << "Expected kind " << ast_statement_kind_to_str(AST_VAR_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind);
        
        std::string expected_ident{tc.expected_ident};
        std::string actual_ident{stmt.stmt.var_statement.ident};
        EXPECT_EQ(expected_ident, actual_ident);

        ast_free_node(&stmt);
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

        EXPECT_EQ(program.len, 1);

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_RETURN_STATEMENT)
            << "Expected kind " << ast_statement_kind_to_str(AST_RETURN_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind);
        
        ast_free_node(&stmt);
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

        EXPECT_EQ(program.len, 1);

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind);

        Expression expr = stmt.stmt.expression_statement.expression;

        EXPECT_EQ(expr.kind, AST_IDENT_EXPRESSION)
            << "Expected kind " << ast_expression_kind_to_str(AST_IDENT_EXPRESSION)
            << ", got " << ast_expression_kind_to_str(expr.kind);

        Ident_Expression iexpr = expr.expr.ident_expression;
        EXPECT_TRUE(strncmp(iexpr.ident, tc.ident, tc.expected_ident_len) == 0)
            << "Expected ident `" << tc.ident 
            << "`, got `" << iexpr.ident << "`";
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

        EXPECT_EQ(program.len, 1);

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind);

        Expression expr = stmt.stmt.expression_statement.expression;

        EXPECT_EQ(expr.kind, AST_INT_EXPRESSION)
            << "Expected kind " << ast_expression_kind_to_str(AST_INT_EXPRESSION)
            << ", got " << ast_expression_kind_to_str(expr.kind);

        Int_Expression iexpr = expr.expr.int_expression;
        EXPECT_EQ(tc.expected_value, iexpr.value);
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

        EXPECT_EQ(program.len, 1);

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind);

        Expression expr = stmt.stmt.expression_statement.expression;

        EXPECT_EQ(expr.kind, AST_FLOAT_EXPRESSION)
            << "Expected kind " << ast_expression_kind_to_str(AST_FLOAT_EXPRESSION)
            << ", got " << ast_expression_kind_to_str(expr.kind);

        Float_Expression fexpr = expr.expr.float_expression;
        EXPECT_FLOAT_EQ(tc.expected_value, fexpr.value);
    }
}

// TODO(HS): decide if I want to allow `!<number>` in parser
// TODO(HS): ^above for strings too
TEST(ParserTestSuite, Parse_Prefix_Expression)
{
    union Number
    {
        int32_t ival;
        float fval;
    };

    struct Test_Case
    {
        const char *input;
        char op;
        Expression_Kind expected_kind;
        Number expected_value;
    };

    std::vector<Test_Case> test_cases{
        { "-10;",  '-', AST_INT_EXPRESSION,   { .ival = 10 }   },
        { "-3.1;", '-', AST_FLOAT_EXPRESSION, { .fval = 3.1f } },
    };

    for (auto &tc : test_cases)
    {
        Lexer l;
        Parser p;

        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);

        EXPECT_EQ(program.len, 1);

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind);

        Expression expr = stmt.stmt.expression_statement.expression;

        EXPECT_EQ(expr.kind, AST_PREFIX_EXPRESSION)
            << "Expected kind " << ast_expression_kind_to_str(AST_PREFIX_EXPRESSION)
            << ", got " << ast_expression_kind_to_str(expr.kind);

        Prefix_Expression pexpr = expr.expr.prefix_expression;
        
        EXPECT_EQ(tc.op, pexpr.op);

        EXPECT_TRUE(pexpr.rhs->kind == tc.expected_kind)
            << "Expected prefix expression operand to be either " 
            << ast_expression_kind_to_str(tc.expected_kind)
            << ", got " << ast_expression_kind_to_str(pexpr.rhs->kind);

        if (pexpr.rhs->kind == AST_INT_EXPRESSION)
        {
            EXPECT_EQ(pexpr.rhs->expr.int_expression.value, tc.expected_value.ival);
        }
        else if (pexpr.rhs->kind == AST_FLOAT_EXPRESSION)
        {
            EXPECT_FLOAT_EQ(pexpr.rhs->expr.float_expression.value, tc.expected_value.fval);
        }

        program_free(&program);
    }
}

// TODO(HS): write function to compare expressions - should compare type and value
TEST(ParserTestSuite, Parse_Binary_Expression)
{
    struct Test_Case
    {
        const char *input;
        char expected_op[2];
        Expression lhs;
        Expression rhs;
    };

    std::vector<Test_Case> test_cases{
        { "5 + 5;",  { '+' }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
        // { "5 - 5;",  { '-' }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
        // { "5 * 5;",  { '*' }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
        // { "5 / 5;",  { '/' }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } }, Expression{ AST_INT_EXPRESSION, { .int_expression = {5} } } },
    };

    for (auto& tc : test_cases)
    {
        Lexer l;
        Parser p;
        
        lexer_init(&l, tc.input);
        parser_init(&p, &l);

        Program program = parser_parse_program(&p);
        EXPECT_EQ(program.len, 1);

        Statement stmt = program.statements[0];
        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind);

        Expression expr = stmt.stmt.expression_statement.expression;
        EXPECT_EQ(expr.kind, AST_INFIX_EXPRESSION)
            << "Expected kind " << ast_expression_kind_to_str(AST_INFIX_EXPRESSION)
            << ", got " << ast_expression_kind_to_str(expr.kind);
        
        Infix_Expression inexpr = expr.expr.infix_expression;

        // test operator
        std::string act_op{inexpr.op};
        std::string exp_op{tc.expected_op};
        EXPECT_EQ(act_op.length(), exp_op.length());
        EXPECT_EQ(act_op, exp_op);

        // test LHS
        Expression *lhs = inexpr.lhs;
        EXPECT_EQ(lhs->kind, tc.lhs.kind)
            << "Expected kind " << ast_expression_kind_to_str(tc.lhs.kind)
            << ", got " << ast_expression_kind_to_str(lhs->kind);

        // test RHS
        Expression *rhs = inexpr.rhs;
        EXPECT_EQ(rhs->kind, tc.rhs.kind)
            << "Expected kind " << ast_expression_kind_to_str(tc.rhs.kind)
            << ", got " << ast_expression_kind_to_str(rhs->kind);

        program_free(&program);
    }
}
