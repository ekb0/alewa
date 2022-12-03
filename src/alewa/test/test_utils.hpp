#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#define ALW_TEST(tname)                                                        \
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

#define ALW_EXPECT_EQ(a, x)                                                    \
    if ((a) != (x)) {                                                          \
        std::string expr = #a + std::string{" == "} + #x;                      \
        fail_expr = alewa::test::pretty_err(expr, a, x);                       \
        return;                                                                \
    }                                                                          \
    static_assert(true, "") /* require a semicolon after using macro */

namespace alewa::test {

struct TestCase {
    std::string name;
    void (*run)(std::string&);
};

static std::vector<TestCase> tests;

template <typename T>
std::string to_str(T const & x)
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
std::string pretty_err(std::string const & expr, T const & a, U const & x)
{
    return "\n" + expr + ":\nActual  : " + to_str(a) + "\nExpected: "
           + to_str(x);
}

}  // namespace alewa::test
