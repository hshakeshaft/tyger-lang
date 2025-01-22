#include <gtest/gtest.h>
#include <vector>
#include <tuple>

#include "tstrings.h"

///
/// character test data
///
const auto whitespace_chars = std::vector<char>{
    '\n', '\r', '\t', ' ', 
};

const auto alpha_chars = std::vector<char>{
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 
};

const auto number_chars = std::vector<char>{
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 
};

const auto puntuation_chars = std::vector<char>{
    ';', ':', ',', '.', 
};


///
/// Tests
///

TEST(String_Functions, String_N_Len)
{
    const char *s = "Hello, World!\n";
    EXPECT_EQ(string_nlen(s, 5), 5);
    EXPECT_EQ(string_nlen(s, 7), 7);
    EXPECT_EQ(string_nlen(s, 10), 10);
    EXPECT_EQ(string_nlen("\0", 10), 0);
    EXPECT_EQ(string_nlen("pub\0fn", 10), 3);
}

TEST(String_Functions, Char_Is_Whitespace)
{
    for (char ch : whitespace_chars) { EXPECT_TRUE(is_whitespace(ch)); }

    for (char ch : alpha_chars) { EXPECT_FALSE(is_whitespace(ch)); }
    for (char ch : number_chars) { EXPECT_FALSE(is_whitespace(ch)); }
    for (char ch : puntuation_chars) { EXPECT_FALSE(is_whitespace(ch)); }
}

TEST(String_Functions, Char_Is_Numeric)
{
    for (char ch : number_chars)     { EXPECT_TRUE(is_numeric(ch)); }

    for (char ch : alpha_chars)      { EXPECT_FALSE(is_numeric(ch)); }
    for (char ch : whitespace_chars) { EXPECT_FALSE(is_numeric(ch)); }
    for (char ch : puntuation_chars) { EXPECT_FALSE(is_numeric(ch)); }

}

TEST(String_Functions, Char_Is_Alphabetic)
{
    for (char ch : alpha_chars)      { EXPECT_TRUE(is_alpha(ch)); }

    for (char ch : whitespace_chars) { EXPECT_FALSE(is_alpha(ch)); }
    for (char ch : number_chars)     { EXPECT_FALSE(is_alpha(ch)); }
    for (char ch : puntuation_chars) { EXPECT_FALSE(is_alpha(ch)); }
}

TEST(String_Functions, Char_Is_Alphanumeric)
{
    for (char ch : alpha_chars)      { EXPECT_TRUE(is_alphanumeric(ch)); }
    for (char ch : number_chars)     { EXPECT_TRUE(is_alphanumeric(ch)); }

    for (char ch : whitespace_chars) { EXPECT_FALSE(is_alphanumeric(ch)); }
    for (char ch : puntuation_chars) { EXPECT_FALSE(is_alphanumeric(ch)); }
}

TEST(String_Functions, Char_Is_Punctuation)
{
    for (char ch : puntuation_chars) { EXPECT_TRUE(is_punctuation(ch)); }

    for (char ch : whitespace_chars) { EXPECT_FALSE(is_punctuation(ch)); }
    for (char ch : alpha_chars)      { EXPECT_FALSE(is_punctuation(ch)); }
    for (char ch : number_chars)     { EXPECT_FALSE(is_punctuation(ch)); }
}

TEST(String_Functions, Char_Is_End_Of_Input)
{
    ASSERT_TRUE(is_end_of_input('\0'));

    for (char ch : whitespace_chars) { EXPECT_FALSE(is_end_of_input(ch)); }
    for (char ch : alpha_chars)      { EXPECT_FALSE(is_end_of_input(ch)); }
    for (char ch : number_chars)     { EXPECT_FALSE(is_end_of_input(ch)); }
    for (char ch : puntuation_chars) { EXPECT_FALSE(is_end_of_input(ch)); }
}

TEST(String_View_Test_Suite, Test_String_View_from_cstr)
{
    const char *str = "pub fn main";
    String_View expected = { (char*) str, 3 };
    auto sv = string_view_from_cstr(str, 3);
    EXPECT_EQ(expected.str,    sv.str);
    EXPECT_EQ(expected.length, sv.length);
}

TEST(String_View_Test_Suite, Test_String_View_from_cstr_offset)
{
    const char *str = "pub fn main";

    auto expected = std::vector<String_View>{
        { (char*) &str[0], 3 },
        { (char*) &str[4], 2 },
        { (char*) &str[7], 4 },
    };

    auto s1 = string_view_from_cstr_offset(str, 0, 3);
    EXPECT_EQ(expected[0].str,    s1.str);
    EXPECT_EQ(expected[0].length, s1.length);

    auto s2 = string_view_from_cstr_offset(str, 4, 2);
    EXPECT_EQ(expected[1].str,    s2.str);
    EXPECT_EQ(expected[1].length, s2.length);

    auto s3 = string_view_from_cstr_offset(str, 7, 4);
    EXPECT_EQ(expected[2].str,    s3.str);
    EXPECT_EQ(expected[2].length, s3.length);
}

TEST(String_View_Test_Suite, Test_String_View_compare)
{
    const char *str = "pub fn main";

    auto s1 = string_view_from_cstr_offset(str, 0, 3);
    auto s2 = string_view_from_cstr_offset(str, 4, 2);
    auto s3 = string_view_from_cstr_offset(str, 7, 4);

    EXPECT_TRUE(string_view_eq(s1, s1));
    EXPECT_TRUE(!string_view_eq(s1, s2));
    EXPECT_TRUE(!string_view_eq(s1, s3));

    EXPECT_FALSE(!string_view_eq(s1, s1));
    EXPECT_FALSE(string_view_eq(s1, s2));
    EXPECT_FALSE(string_view_eq(s1, s3));
}

#include <stdio.h>

TEST(String_View_Test_Suite, Test_String_View_compare_cstr)
{
    auto test_eq = [] (String_View actual, const char *expected, bool should_match = true) {
        char buff[10];
        snprintf(buff, sizeof(buff), sv_fmt, sv_args(actual));

        if (should_match)
        {
            EXPECT_TRUE(string_view_eq_cstr(actual, expected))
                << "Expected String_View \"" << buff 
                << "\" to match string \"" << expected << "\"";
        }
        else
        {
            EXPECT_FALSE(string_view_eq_cstr(actual, expected))
                << "Expected String_View \"" << buff 
                << "\" not to match string \"" << expected << "\"";
        }
    };

    const char *str = "pub fn main";

    String_View sv = string_view_from_cstr_offset(str, 0, 3); 
    test_eq(sv, "pubn", false);

    sv = string_view_from_cstr_offset(str, 0, 3); 
    test_eq(sv, "pub");
    test_eq(sv, "bup", false);

    sv = string_view_from_cstr_offset(str, 4, 2); 
    test_eq(sv, "fn");

    sv = string_view_from_cstr_offset(str, 7, 4); 
    test_eq(sv, "main");
}
