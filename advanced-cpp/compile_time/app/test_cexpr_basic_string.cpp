#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>
#include "ra/cexpr_basic_string.hpp"

constexpr bool string_cmp(const char *a, const char *b) {
    while (*a == *b) {
        if ((*a == '\0' && *b == '\0')) {
            return true;
        }

        ++a; ++b;
    }

    return false;
}

TEST_CASE("Default Constructor") {
    constexpr ra::cexpr::cexpr_basic_string<char, 4> str1;
    REQUIRE(str1.size() == 0);
    REQUIRE(*str1.data() == '\0');
    REQUIRE(str1.max_size() == 4);
    REQUIRE(str1.capacity() == 4);
    REQUIRE(*str1.begin() == '\0');
    REQUIRE(*str1.end() == '\0');
    REQUIRE(str1.begin() == str1.end());

    ra::cexpr::cexpr_basic_string<char, 4> str2;
    REQUIRE(*str2.data() == '\0');
    REQUIRE(*str2.begin() == '\0');
    REQUIRE(*str2.end() == '\0');
    REQUIRE(str2.begin() == str2.end());
}

TEST_CASE("Iterator constructor") {
    SECTION("Non-Full String") {
        constexpr char strlit1[] = "Hello";
        // Not full string
        constexpr ra::cexpr::cexpr_basic_string<char, 10> str2(strlit1, strlit1 + 4);
        REQUIRE(str2.size() == 4);
        REQUIRE(str2[0] == 'H');
        REQUIRE(str2[3] == 'l');
        REQUIRE(str2[4] == '\0');
        REQUIRE(*(str2.end() - 1) == 'l');
        REQUIRE_THROWS_AS(str2[5], std::runtime_error);
    }

    SECTION("Single Char Iter") {
        constexpr char strlit1[] = "Hello";
        constexpr ra::cexpr::cexpr_basic_string<char, 0> str2(strlit1, strlit1);
        REQUIRE(str2.size() == 0);
        REQUIRE(str2[str2.size()] == '\0');
        REQUIRE_THROWS_AS(str2[1], std::runtime_error);
    }

    SECTION("Full String") {
        constexpr char strlit1[] = "Hello World!";
        constexpr ra::cexpr::cexpr_basic_string<char, 12> str2(strlit1, strlit1 + 12);
        REQUIRE(str2.size() == 12);
        REQUIRE(str2[str2.size()] == '\0');
        REQUIRE_THROWS_AS(str2[str2.size() + 1], std::runtime_error);
    }

    SECTION("Overflow") {
        constexpr char strlit1[] = "Hello World!";
        REQUIRE_THROWS(ra::cexpr::cexpr_basic_string<char, 11>(strlit1, strlit1 + 12));
        REQUIRE_NOTHROW(ra::cexpr::cexpr_basic_string<char, 12>(strlit1, strlit1 + 12));
        REQUIRE_NOTHROW(ra::cexpr::cexpr_basic_string<char, 13>(strlit1, strlit1 + 12));
    }
}

TEST_CASE("T * constructor") {
    SECTION("Non-Full String") {
        constexpr char strlit1[] = "Hello";
        // Not full string
        constexpr ra::cexpr::cexpr_basic_string<char, 10> str2(strlit1);
        REQUIRE(str2.size() == 5);
        REQUIRE(str2[0] == 'H');
        REQUIRE(str2[4] == 'o');
        REQUIRE(str2[str2.size()] == '\0');
        REQUIRE_THROWS_AS(str2[6], std::runtime_error);
    }

    SECTION("Empty String") {
        constexpr char strlit1[] = "";
        constexpr ra::cexpr::cexpr_basic_string<char, 0> str2(strlit1);
        REQUIRE(str2.size() == 0);
        REQUIRE(str2[0] == '\0');
        REQUIRE_THROWS_AS(str2[1], std::runtime_error);
    }

    SECTION("Full String") {
        constexpr char strlit1[] = "Hello World!";
        constexpr ra::cexpr::cexpr_basic_string<char, 12> str2(strlit1);
        REQUIRE(str2.size() == 12);
        REQUIRE(str2[str2.size()] == '\0');
        REQUIRE_THROWS_AS(str2[str2.size() + 1], std::runtime_error);
    }

    SECTION("Overflow") {
        constexpr char strlit1[] = "Hello World!";
        REQUIRE_THROWS(ra::cexpr::cexpr_basic_string<char, 11>(strlit1));
        REQUIRE_NOTHROW(ra::cexpr::cexpr_basic_string<char, 12>(strlit1));
        REQUIRE_NOTHROW(ra::cexpr::cexpr_basic_string<char, 13>(strlit1));
    }
}

TEST_CASE("Push And Pops") {
    SECTION("Push") {
        constexpr char strlit1[] = "Hello Worl";
        constexpr char strlit2[] = "d!";
        ra::cexpr::cexpr_basic_string<char, 11> str1(strlit1);
        REQUIRE_NOTHROW(str1.push_back(strlit2[0]));
        REQUIRE(str1[10] == 'd');
        REQUIRE(str1[11] == '\0');
        REQUIRE_THROWS_AS(str1.push_back(strlit2[1]), std::runtime_error);
        REQUIRE(str1[11] == '\0');
    }

    SECTION("Pop") {
        constexpr char strlit1[] = "H";
        ra::cexpr::cexpr_basic_string<char, 2> str1(strlit1);
        REQUIRE(str1[1] == '\0');
        REQUIRE(str1.size() == 1);
        REQUIRE_NOTHROW(str1.pop_back());
        REQUIRE(str1[0] == '\0');
        REQUIRE(str1.size() == 0);
        REQUIRE_THROWS_AS(str1.pop_back(), std::runtime_error);
    }
}

TEST_CASE("Append a string to another string") {
    ra::cexpr::cexpr_basic_string<char, 5> str1("");
    REQUIRE(str1[0] == '\0');
    str1.append("Hello");
    REQUIRE(str1[str1.size()] == '\0');
    REQUIRE(string_cmp(str1.data(), "Hello"));

    REQUIRE_THROWS_AS(str1.append(nullptr), std::runtime_error);
    REQUIRE_THROWS_AS(str1.append("!"), std::runtime_error);

    ra::cexpr::cexpr_basic_string<char, 10> str2("Hello");
    str2.append(str2.data());
    REQUIRE(string_cmp(str2.data(), "HelloHello"));
}

TEST_CASE("Append cexpr_basis to another one") {
    ra::cexpr::cexpr_basic_string<char, 5> str1("");
    ra::cexpr::cexpr_basic_string<char, 10> str2("Hello");
    str1.append(str2);
    
    REQUIRE(str1.size() == 5);
    REQUIRE(string_cmp(str1.data(), "Hello"));
    ra::cexpr::cexpr_basic_string<char, 10> str3("!");
    REQUIRE_THROWS_AS(str1.append(str3), std::runtime_error);
    
    str2.append(str2);
    REQUIRE(string_cmp(str2.data(), "HelloHello"));
    REQUIRE(str2.size() == 10);

    ra::cexpr::cexpr_basic_string<char, 5> str4("");
    str2.append(str4);
    REQUIRE(string_cmp(str2.data(), "HelloHello"));
    REQUIRE(str2.size() == 10);
}

TEST_CASE("Convert to a string") {
    char buff[4] = {};
    std::size_t val = ra::cexpr::to_string(123, buff, 4, nullptr);
    REQUIRE(std::string(buff) == std::string("123"));
    REQUIRE(val == 3);

    REQUIRE_THROWS(ra::cexpr::to_string(1233, buff, 4, nullptr));
    REQUIRE_THROWS(ra::cexpr::to_string(123, buff, 3, nullptr));

    char *end = nullptr;
    ra::cexpr::to_string(123, buff, 4, &end);
    REQUIRE(*end == '\0');
    REQUIRE(*(end - 1) == '3');
}