#define BOOST_TEST_MODULE urlencode tests
#include <boost/test/unit_test.hpp>

#include "../src/urldecode.h"

BOOST_AUTO_TEST_CASE(EmptyString) {
    using namespace std::literals;
    BOOST_TEST(UrlDecode(""sv).empty());
}

BOOST_AUTO_TEST_CASE(ValidString) {
    const std::string str{ "string-string_string" };
    const std::string expected{ "string-string_string" };
    BOOST_REQUIRE_EQUAL(UrlDecode(str), expected);
}

BOOST_AUTO_TEST_CASE(StringWithPrc) {
    const std::string str{ "127.0.0.1%3A8080%2Fapi%2Fv1%2Fgame%2Fjoin" };
    const std::string expected{ "127.0.0.1:8080/api/v1/game/join" };
    BOOST_REQUIRE_EQUAL(UrlDecode(str), expected);
}

BOOST_AUTO_TEST_CASE(InvalidString) {
    const std::string invalidStr{ "string%2gstring+string" };
    BOOST_REQUIRE_THROW(UrlDecode(invalidStr), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(StringPlus) {
    const std::string str{ "string+string+string" };
    const std::string expected{ "string string string" };
    BOOST_REQUIRE_EQUAL(UrlDecode(str), expected);
}