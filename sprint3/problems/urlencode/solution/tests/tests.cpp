#include <gtest/gtest.h>

#include "../src/urldecode.h"

TEST(TestSuite, EmptyString) {
    EXPECT_TRUE(UrlEncode(""sv).empty());
}

TEST(TestSuite, NothingToEncode) {
    const std::string orig{ "string-string_string" };
    const std::string expected{ "string-string_string" };
    EXPECT_EQ(UrlEncode(orig), expected);
}

TEST(TestSuite, ComplexCharsAreEncoded) {
    const std::string orig{ "127.0.0.1:8080/api/v1/game/join" };
    const std::string expected{ "127.0.0.1%3A8080%2Fapi%2Fv1%2Fgame%2Fjoin" };
    EXPECT_EQ(UrlEncode(orig), expected);
}

TEST(TestSuite, StringWithSpaces) {
    const std::string orig{ "string string string" };
    const std::string expected{ "string+string+string" };
    EXPECT_EQ(UrlEncode(orig), expected);
}