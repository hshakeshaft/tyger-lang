/**
 * parser_test_util.hpp
 * 
 * Header only library containing utility functions for testing parser functionality,
 * particularly expression parsing.
 * 
 * To use this anywhere, include the header file and define the following in 
 * **one and only one** source file:
 * 
 *  #define PARSER_TEST_UTIL_IMPL
 * 
*/
#ifndef PARSER_TEST_UTIL_HPP_
#define PARSER_TEST_UTIL_HPP_
#include <gtest/gtest.h>
#include "parser.h"

/// Tests that 2 expressions match, prints AST if not
void test_expression(Expression exp, Expression act, const char *prog_str);

/// Tests that 2 ident_expressios match (i.e. idents are the same), prints AST if not
void test_ident_expression(Expression exp, Expression act, const char *prog_str);

/// Tests that 2 int_expressions match, prints AST if not
void test_int_expression(Expression exp, Expression act, const char *prog_str);

/// Tests that 2 float_expressions match, prints AST if not
void test_float_expression(Expression exp, Expression act, const char *prog_str);

/// Tests that 2 boolean_expressions match, prints AST if not
void test_boolean_expression(Expression exp, Expression act, const char *prog_str);

/// Tests that 2 prefix_expressions match, prints AST if not
void test_prefix_expression(Expression exp, Expression act, const char *prog_str);

/// Tests that 2 infix_expression match, prints AST if not
void test_infix_expression(Expression exp, Expression act, const char *prog_str);

#ifdef PARSER_TEST_UTIL_IMPL

void test_expression(Expression exp, Expression act, const char *prog_str)
{
    EXPECT_EQ(exp.kind, act.kind)
        << "Expected expression of kind " << ast_expression_kind_to_str(exp.kind)
        << ", got" << ast_expression_kind_to_str(act.kind)
        << "\n" << prog_str;

    switch (exp.kind)
    {
        case AST_IDENT_EXPRESSION:   { test_ident_expression(exp, act, prog_str); } break;
        case AST_INT_EXPRESSION:     { test_int_expression(exp, act, prog_str); } break;
        case AST_FLOAT_EXPRESSION:   { test_float_expression(exp, act, prog_str); } break;
        case AST_BOOLEAN_EXPRESSION: { test_boolean_expression(exp, act, prog_str); } break;
        case AST_PREFIX_EXPRESSION:  { test_prefix_expression(exp, act, prog_str); } break;
        case AST_INFIX_EXPRESSION:   { test_infix_expression(exp, act, prog_str); } break;
    }
}

void test_ident_expression(Expression exp, Expression act, const char *prog_str)
{
    EXPECT_EQ(exp.kind, act.kind) 
        << "Expected expression kind " << ast_expression_kind_to_str(exp.kind)
        << ", got " << ast_expression_kind_to_str(act.kind)
        << "\n" << prog_str;

    std::string exp_ident{exp.expr.ident_expression.ident};
    std::string act_ident{act.expr.ident_expression.ident};
    EXPECT_EQ(exp_ident, act_ident) << prog_str;
}

void test_int_expression(Expression exp, Expression act, const char *prog_str)
{
    EXPECT_EQ(exp.kind, act.kind) 
        << "Expected expression kind " << ast_expression_kind_to_str(exp.kind)
        << ", got " << ast_expression_kind_to_str(act.kind)
        << "\n" << prog_str;

    EXPECT_EQ(exp.expr.int_expression.value, act.expr.int_expression.value);
}

void test_float_expression(Expression exp, Expression act, const char *prog_str)
{
    EXPECT_EQ(exp.kind, act.kind) 
        << "Expected expression kind " << ast_expression_kind_to_str(exp.kind)
        << ", got " << ast_expression_kind_to_str(act.kind)
        << "\n" << prog_str;

    EXPECT_FLOAT_EQ(exp.expr.int_expression.value, act.expr.int_expression.value);
}

void test_boolean_expression(Expression exp, Expression act, const char *prog_str)
{
    EXPECT_EQ(exp.kind, act.kind) 
        << "Expected expression kind " << ast_expression_kind_to_str(exp.kind)
        << ", got " << ast_expression_kind_to_str(act.kind)
        << "\n" << prog_str;

    EXPECT_FLOAT_EQ(exp.expr.boolean_expression.value, act.expr.boolean_expression.value)
        << prog_str;
}

void test_prefix_expression(Expression exp, Expression act, const char *prog_str)
{
    EXPECT_EQ(exp.expr.prefix_expression.op, act.expr.prefix_expression.op)
        << "Operators did not match \n" << prog_str;
    
    test_expression(*exp.expr.prefix_expression.rhs, *act.expr.prefix_expression.rhs, prog_str);
}

void test_infix_expression(Expression exp, Expression act, const char *prog_str)
{
    Token_Kind exp_op = exp.expr.infix_expression.op;
    Token_Kind act_op = act.expr.infix_expression.op;

    EXPECT_EQ(exp_op, act_op)
        << "Expected operator " << token_kind_to_string(exp_op)
        << ", got " << token_kind_to_string(act_op)
        << "\n" << prog_str;
    
    test_expression(*exp.expr.prefix_expression.rhs, *act.expr.prefix_expression.rhs, prog_str);
}

#endif

#endif // PARSER_TEST_UTIL_HPP_
