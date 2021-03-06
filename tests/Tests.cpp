#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>

#include "../String.hpp"

TEST_CASE("Construction")
{
    unicpp::string str1("This is some text!");
    REQUIRE(str1.std_str() == "This is some text!");

    unicpp::string empty;
    REQUIRE(empty.std_str().empty());

    char data[12] = { 'h', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
    unicpp::string fixed_sized(data, 12);
    REQUIRE(fixed_sized.std_str() == "hello world!");
    REQUIRE(fixed_sized.std_str().size() == 12);

    unicpp::string repeated(10, U'c');
    REQUIRE(repeated.std_str() == "cccccccccc");
}

TEST_CASE("Conversion from/to UTF32")
{
    unicpp::string utf8str(U"Elegant, 时尚, élégant, 🞊");
    REQUIRE(utf8str.std_str() == u8"Elegant, 时尚, élégant, 🞊");

    REQUIRE(utf8str.utf32_str() == U"Elegant, 时尚, élégant, 🞊");
    REQUIRE(unicpp::string(utf8str.utf32_str()).std_str() == u8"Elegant, 时尚, élégant, 🞊");

    // Not encodable codepoint
    REQUIRE_THROWS_AS(unicpp::string unencStr(U"Bad codepoint: \x0023F1AF <-- here!"), unicpp::not_encodable_codepoint_exception);
}

unicpp::string testing_strings[] = {
    // Valid:
    u8"Elegant, 时尚, élégant, 🞊",
    // Contains an invalid octet at the beginning of a "character"
    u8"This contains an \xC0\x11 character",
    // Contains an invalid octet in a "character"
    u8"This contains an \xC1\xC0 character",
    // Contains an invalid sequence lead character 
    // (not 0xxxxxxx, 110xxxxx, 1110xxxx or 11110xxx)
    u8"This contains an \xFC\x11 character",
    // Contains lead character of 10xxxxxx (which should
    // never be a head octet but is always a following octet
    u8"This contains an \xA7\x11 character",
    // Contains a 3 three byte sequence but with one octet missing
    u8"This contains an \xEC\xAC character",
    // Contains a 3 three byte sequence --> VALID
    u8"This contains an \xEC\xAC\xAD character",
    // Contains a 3 three byte sequence but with 4 octets!
    u8"This contains an \xEC\xAC\xAF\xAD character",
    // Contains a 3 three byte sequence but with one octet missing because the string is not long enough
    u8"This contains an \xEC\xAC",
};

TEST_CASE("Exceptions when browsing a utf8 string")
{
    //Use the conversion to UTF32 to do the test as it browses the whole utf8 string.
    REQUIRE_NOTHROW(testing_strings[0].utf32_str());
    REQUIRE_THROWS_AS(testing_strings[1].utf32_str(), unicpp::invalid_utf8_exception);
    REQUIRE_THROWS_AS(testing_strings[2].utf32_str(), unicpp::invalid_utf8_exception);
    REQUIRE_THROWS_AS(testing_strings[3].utf32_str(), unicpp::bad_utf8_sequence_exception);
    REQUIRE_THROWS_AS(testing_strings[4].utf32_str(), unicpp::bad_utf8_sequence_exception);
    REQUIRE_THROWS_AS(testing_strings[5].utf32_str(), unicpp::bad_utf8_sequence_exception);
    REQUIRE_NOTHROW(testing_strings[6].utf32_str());
    REQUIRE_THROWS_AS(testing_strings[7].utf32_str(), unicpp::bad_utf8_sequence_exception);
    REQUIRE_THROWS_AS(testing_strings[8].utf32_str(), unicpp::bad_utf8_sequence_exception);
}

TEST_CASE("string::is_valid")
{
    REQUIRE(testing_strings[0].is_valid() == true);
    REQUIRE(testing_strings[1].is_valid() == false);
    REQUIRE(testing_strings[2].is_valid() == false);
    REQUIRE(testing_strings[3].is_valid() == false);
    REQUIRE(testing_strings[4].is_valid() == false);
    REQUIRE(testing_strings[5].is_valid() == false);
    REQUIRE(testing_strings[6].is_valid() == true);
    REQUIRE(testing_strings[7].is_valid() == false);
    REQUIRE(testing_strings[8].is_valid() == false);
}
