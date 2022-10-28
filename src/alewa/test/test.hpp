#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace alewa::test {

struct TestCase {
    std::string name;
    void (*run)(std::string&);
};

static std::vector<TestCase> tests;

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
    void (_test_##tname)(std::string& fail_expr)

#define EXPECT(cond)                                                           \
    if (!(cond)) {                                                             \
        fail_expr = #cond;                                                     \
        return;                                                                \
    }                                                                          \
    static_assert(true, "") /* require a semicolon after using macro */
