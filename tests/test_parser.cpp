#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <stdint.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "trace.h"

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
        const char *prog_str = program_print_ast(&program, PRINT_FORMAT_YAML);

        EXPECT_EQ(program.len, 1) << prog_str;

        Statement stmt = program.statements[0];

        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT) 
            << "Expected kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind)
            << "\n" << prog_str;

        Expression expr = stmt.stmt.expression_statement.expression;

        EXPECT_EQ(expr.kind, AST_PREFIX_EXPRESSION)
            << "Expected kind " << ast_expression_kind_to_str(AST_PREFIX_EXPRESSION)
            << ", got " << ast_expression_kind_to_str(expr.kind)
            << "\n" << prog_str;

        Prefix_Expression pexpr = expr.expr.prefix_expression;
        
        EXPECT_EQ(tc.op, pexpr.op) << prog_str;

        EXPECT_EQ(pexpr.rhs->kind, tc.expected_kind)
            << "Expected prefix expression operand to be either " 
            << ast_expression_kind_to_str(tc.expected_kind)
            << ", got " << ast_expression_kind_to_str(pexpr.rhs->kind)
            << "\n" << prog_str;

        if (pexpr.rhs->kind == AST_INT_EXPRESSION)
        {
            EXPECT_EQ(pexpr.rhs->expr.int_expression.value, tc.expected_value.ival) << prog_str;
        }
        else if (pexpr.rhs->kind == AST_FLOAT_EXPRESSION)
        {
            EXPECT_FLOAT_EQ(pexpr.rhs->expr.float_expression.value, tc.expected_value.fval) << prog_str;
        }

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

// TODO(HS): validate with different nesting - mimic "ast debug" print used in the
// interpreter book. Entirely because manually building as I am doing is **PAINFUL!**.
TEST(ParserTestSuite, Parse_Nested_Binary_Expression)
{
    struct Test_Case
    {
        const char *input;
        Expression expected;
    };

    // 5 + 4 + 3
    Expression lhs1_lhs{ AST_INT_EXPRESSION, { .int_expression = { 5 } } };
    Expression lhs1_rhs{ AST_INT_EXPRESSION, { .int_expression = { 4 } } };
    Expression lhs1{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_PLUS, &lhs1_lhs, &lhs1_rhs }} };
    Expression rhs1{ AST_INT_EXPRESSION, { .int_expression = { 3 } }};
    Expression expr1{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_PLUS, &lhs1, &rhs1 } } };

    // 5 - 4 - 3
    Expression lhs2_lhs{ AST_INT_EXPRESSION, { .int_expression = { 5 } } };
    Expression lhs2_rhs{ AST_INT_EXPRESSION, { .int_expression = { 4 } } };
    Expression lhs2{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_MINUS, &lhs2_lhs, &lhs2_rhs }} };
    Expression rhs2{ AST_INT_EXPRESSION, { .int_expression = { 3 } }};
    Expression expr2{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_MINUS, &lhs2, &rhs2 } } };

    // 5 + 4 - 3
    Expression lhs3_lhs{ AST_INT_EXPRESSION, { .int_expression = { 5 } } };
    Expression lhs3_rhs{ AST_INT_EXPRESSION, { .int_expression = { 4 } } };
    Expression lhs3{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_PLUS, &lhs3_lhs, &lhs3_rhs }} };
    Expression rhs3{ AST_INT_EXPRESSION, { .int_expression = { 3 } }};
    Expression expr3{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_MINUS, &lhs3, &rhs3 } } };

    // 5 - 4 + 3
    Expression lhs4_lhs{ AST_INT_EXPRESSION, { .int_expression = { 5 } } };
    Expression lhs4_rhs{ AST_INT_EXPRESSION, { .int_expression = { 4 } } };
    Expression lhs4{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_MINUS, &lhs4_lhs, &lhs4_rhs }} };
    Expression rhs4{ AST_INT_EXPRESSION, { .int_expression = { 3 } }};
    Expression expr4{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_PLUS, &lhs4, &rhs4 } } };

    // 5 + 4 * 3
    Expression lhs5{ AST_INT_EXPRESSION, { .int_expression = { 5 } } };
    Expression rhs5_lhs{ AST_INT_EXPRESSION, { .int_expression = { 4 } } };
    Expression rhs5_rhs{ AST_INT_EXPRESSION, { .int_expression = { 3 } } };
    Expression rhs5{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_ASTERISK, &rhs5_lhs, &rhs5_rhs } } };
    Expression expr5{ AST_INFIX_EXPRESSION, { .infix_expression = { TK_PLUS, &lhs5, &rhs5 } }};

    std::vector<Test_Case> test_cases{
        { "5 + 4 + 3;", expr1 },
        { "5 - 4 - 3;", expr2 },
        { "5 + 4 - 3;", expr3 },
        { "5 - 4 + 3;", expr4 },
        { "5 + 4 * 3;", expr5 },
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
        EXPECT_EQ(expr.kind, tc.expected.kind)
            << "Expected kind " << ast_expression_kind_to_str(tc.expected.kind)
            << ", got " << ast_expression_kind_to_str(expr.kind)
            << "\n" << prog_str;

        Infix_Expression inexpr1 = expr.expr.infix_expression;
        Infix_Expression tc_expr = expr.expr.infix_expression;

        EXPECT_EQ(inexpr1.op, tc_expr.op) 
            << "Expected operator " << token_kind_to_string(tc_expr.op)
            << ", got " << token_kind_to_string(inexpr1.op)
            << "\n" << prog_str;

        { // validate LHS
            EXPECT_EQ(inexpr1.lhs->kind, tc_expr.lhs->kind)
                << "Expected LHS kind " << ast_expression_kind_to_str(tc_expr.lhs->kind)
                << ", got " << ast_expression_kind_to_str(inexpr1.lhs->kind)
                << "\n" << prog_str;

            if (inexpr1.lhs->kind == AST_INT_EXPRESSION)
            {
                EXPECT_EQ(inexpr1.lhs->expr.int_expression.value, tc_expr.lhs->expr.int_expression.value)
                    << prog_str;
            }
            else if (inexpr1.lhs->kind == AST_INFIX_EXPRESSION)
            {
                Token_Kind act_op  = inexpr1.lhs->expr.infix_expression.op;
                Expression *act_lhs = inexpr1.lhs->expr.infix_expression.lhs;
                Expression *act_rhs = inexpr1.lhs->expr.infix_expression.rhs;

                Token_Kind exp_op  = tc_expr.lhs->expr.infix_expression.op;
                Expression *exp_lhs = tc_expr.lhs->expr.infix_expression.lhs;
                Expression *exp_rhs = tc_expr.lhs->expr.infix_expression.rhs;

                EXPECT_EQ(act_op, exp_op)
                    << "Expected operator " << token_kind_to_string(exp_op)
                    << ", got " << token_kind_to_string(act_op)
                    << "\n" << prog_str;
                
                EXPECT_EQ(act_lhs->kind, exp_lhs->kind) 
                    << "Expected kind " << ast_expression_kind_to_str(exp_lhs->kind)
                    << ", got " << ast_expression_kind_to_str(act_lhs->kind)
                    << "\n" << prog_str;

                EXPECT_EQ(act_lhs->expr.int_expression.value, exp_lhs->expr.int_expression.value) 
                    << prog_str;

                EXPECT_EQ(act_rhs->kind, exp_rhs->kind) 
                    << "Expected operator " << token_kind_to_string(exp_op)
                    << ", got " << token_kind_to_string(act_op)
                    << "\n" << prog_str;

                EXPECT_EQ(act_rhs->expr.int_expression.value, exp_rhs->expr.int_expression.value) 
                    << prog_str;
            }
        }

        { // validate RHS
            EXPECT_EQ(inexpr1.rhs->kind, tc_expr.rhs->kind)
                << "Expected RHS kind " << ast_expression_kind_to_str(tc_expr.rhs->kind)
                << ", got " << ast_expression_kind_to_str(inexpr1.rhs->kind)
                << "\n" << prog_str;

            if (inexpr1.rhs->kind == AST_INT_EXPRESSION)
            {
                EXPECT_EQ(inexpr1.rhs->expr.int_expression.value, tc_expr.rhs->expr.int_expression.value)
                    << prog_str;
            }
            else if (inexpr1.rhs->kind == AST_INFIX_EXPRESSION)
            {
                Token_Kind act_op   = inexpr1.rhs->expr.infix_expression.op;
                Expression *act_lhs = inexpr1.rhs->expr.infix_expression.lhs;
                Expression *act_rhs = inexpr1.rhs->expr.infix_expression.rhs;

                Token_Kind exp_op   = tc_expr.rhs->expr.infix_expression.op;
                Expression *exp_lhs = tc_expr.rhs->expr.infix_expression.lhs;
                Expression *exp_rhs = tc_expr.rhs->expr.infix_expression.rhs;

                EXPECT_EQ(act_op, exp_op)
                    << "Expected operator " << token_kind_to_string(exp_op)
                    << ", got " << token_kind_to_string(act_op)
                    << "\n" << prog_str;
                
                EXPECT_EQ(act_lhs->kind, exp_lhs->kind) 
                    << "Expected kind " << ast_expression_kind_to_str(exp_lhs->kind)
                    << ", got " << ast_expression_kind_to_str(act_lhs->kind)
                    << "\n" << prog_str;

                EXPECT_EQ(act_lhs->expr.int_expression.value, exp_lhs->expr.int_expression.value) 
                    << prog_str;

                EXPECT_EQ(act_rhs->kind, exp_rhs->kind) 
                    << "Expected operator " << token_kind_to_string(exp_op)
                    << ", got " << token_kind_to_string(act_op)
                    << "\n" << prog_str;

                EXPECT_EQ(act_rhs->expr.int_expression.value, exp_rhs->expr.int_expression.value) 
                    << prog_str;
            }
        }

        program_free(&program);
        free((void *) prog_str);
    }
}
