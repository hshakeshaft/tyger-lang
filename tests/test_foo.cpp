#include <gtest/gtest.h>
#include <vector>
#include "foo.h"

TEST(FooTestSet, Test_Squarei)
{
    struct TestCase
    {
        int input;
        int expected;
    };

    auto tests = std::vector<TestCase>{
        { 1, 1},
        { 2, 4},
        { 3, 9},
        {-2, 4},
        { 0, 0},
    };

    for (auto t : tests)
    {
        EXPECT_EQ(t.expected, squarei(t.input));
    }
}
