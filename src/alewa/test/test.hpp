#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

namespace alewa::test {

struct TestCase {
    std::string name;
    void (*run)(std::string&);
};

static std::vector<TestCase> tests;

template <typename T>
std::string tostr(T const & x)
{
    if constexpr (std::is_convertible_v<T, std::string>) {
        return std::string{x};
    }
    else {
        std::ostringstream ss;
        char s[10];
        for (size_t i = 0; i < sizeof(x); ++i) {
            sprintf(s, "%02x", (unsigned int) ((char*)&x)[i]);
            ss << s;
        }
        return ss.str();
    }
}

template <typename T, typename U>
std::string pretty_err(std::string const & expr, T const & x, U const & y)
{
    return "\n" + expr + ":\nExpected: " + tostr(x) + "\nActual: " + tostr(y);
}

}  // namespace alewa::test

#define TEST(tname)                                                            \
    void (_test_##tname)(std::string&);                                        \
                                                                               \
    struct _add_test_##tname                                                   \
    {                                                                          \
        _add_test_##tname()                                                    \
        {                                                                      \
            alewa::test::tests.push_back({#tname, _test_##tname});             \
        }                                                                      \
    };                                                                         \
                                                                               \
    static _add_test_##tname call_add_test_##tname;                            \
                                                                               \
    void (_test_##tname)(std::string& fail_expr [[maybe_unused]])

#define EXPECT_EQ(x, y)                                                        \
    if (x != y) {                                                              \
        std::string expr = #x + std::string{" == "} + #y;                      \
        fail_expr = alewa::test::pretty_err(expr, x, y);                       \
        return;                                                                \
    }                                                                          \
    static_assert(true, "") /* require a semicolon after using macro */

#define EXPECT_NEQ(x, y)                                                       \
    if (x == y) {                                                              \
        std::string expr = #x + std::string{" != "} + #y;                      \
        fail_expr = alewa::test::pretty_err(expr, x, y);                       \
        return;                                                                \
    }                                                                          \
    static_assert(true, "") /* require a semicolon after using macro */

#define BUG(msg)                                                               \
    fail_expr = msg;                                                           \
    return
