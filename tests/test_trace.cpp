#include <gtest/gtest.h>

#include <vector>
#include <string>

#include "parser.h"
#include "trace.h"

///
/// Test trace with `plain` formatting prints correctly ///
///

TEST(TracePlainTestSuite, Test_Trace_Var_Statement)
{
    struct Test_Case
    {
        const char *input;
        const char *expected_ast;
    };

    std::vector<Test_Case> test_cases{
        { "var x = 10;",       "(var x (10))" },
        { "var PI = 3.14159;", "(var PI (3.141590))" },

        { "var x = 5 + 4 * 3;", "(var x (5 + (4 * 3)))" },
        { "var x = 5 + 4 * 3 / 2 - 1;", "(var x ((5 + ((4 * 3) / 2)) - 1))" },
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

        EXPECT_EQ(program.len, 1) << prog_str << "\n" << prog_yml;

        Statement stmt = program.statements[0];
        EXPECT_EQ(stmt.kind, AST_VAR_STATEMENT)
            << "Expected statement kind " << ast_statement_kind_to_str(AST_VAR_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind)
            << "\n" << prog_yml;

        std::string act_ast{prog_str};
        std::string exp_ast{tc.expected_ast};
        EXPECT_EQ(act_ast, exp_ast);

        program_free(&program);
        free((void *) prog_str);
        free((void *) prog_yml);
    }
}


TEST(TracePlainTestSuite, Test_Trace_Return_Statement)
{}

TEST(TracePlainTestSuite, Test_Trace_Expression_Statement)
{
    struct Test_Case
    {
        const char *input;
        const char *expected_ast;
    };

    std::vector<Test_Case> test_cases{
        { "5;", "(5)" },

        // NOTE(HS): trailing 0 bodge because I don't want to code the switch to check this
        { "3.14159;", "(3.141590)" },

        { "a;", "(a)" },

        { "-a;", "((-a))" },

        { "-a + b;", "((-a) + b)" },
        { "5 + 4;",     "(5 + 4)" },
        { "5 + 4 + 3;", "((5 + 4) + 3)" },
        { "5 + 4 - 3;", "((5 + 4) - 3)" },
        { "5 + 4 * 3;", "(5 + (4 * 3))" },

        { "false;", "(false)" },
        { "!true;", "((!true))" },
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

        EXPECT_EQ(program.len, 1) << prog_str << "\n" << prog_yml;

        Statement stmt = program.statements[0];
        EXPECT_EQ(stmt.kind, AST_EXPRESSION_STATEMENT)
            << "Expected statement kind " << ast_statement_kind_to_str(AST_EXPRESSION_STATEMENT)
            << ", got " << ast_statement_kind_to_str(stmt.kind)
            << "\n" << prog_yml;

        std::string act_ast{prog_str};
        std::string exp_ast{tc.expected_ast};
        EXPECT_EQ(act_ast, exp_ast);

        program_free(&program);
        free((void *) prog_str);
        free((void *) prog_yml);
    }
}
