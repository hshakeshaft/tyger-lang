#include <gtest/gtest.h>
#include <vector>
#include <string>

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
