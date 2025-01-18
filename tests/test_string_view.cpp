#include <gtest/gtest.h>
#include <vector>
#include <tuple>

#include "tstrings.h"

TEST(String_Functions, String_N_Len)
{
    const char *s = "Hello, World!\n";
    EXPECT_EQ(string_nlen(s, 5), 5);
    EXPECT_EQ(string_nlen(s, 7), 7);
    EXPECT_EQ(string_nlen(s, 10), 10);
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
    char buff[10];
    const char *str = "pub fn main";

    const char *tc0 = "pubn";
    const char *tc1 = "pub";
    // const char *tc2 = "fn";
    // const char *tc3 = "main";

    String_View s1 = string_view_from_cstr_offset(str, 0, 3); 

    snprintf(buff, sizeof(buff), sv_fmt, sv_args(s1));
    EXPECT_FALSE(string_view_eq_cstr(s1, tc0))
        << "Expected String_View \"" << buff 
        << "\" not to match string \"" << tc0 << "\"";

    EXPECT_TRUE(string_view_eq_cstr(s1, tc1))
        << "Expected String_View \"" << buff 
        << "\" to match string \"" << tc1 << "\"";
}
