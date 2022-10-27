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

static size_t run_all_tests()
{
    size_t passed = 0;

    for (auto const & test : tests) {
        std::string fail_expr{};
        test.run(fail_expr);
        if (fail_expr.empty()) {
            std::cout << "[ PASS ] " << test.name << std::endl;
            ++passed;
        }
        else {
            std::cout << "[ FAIL ] " << test.name << ": " << fail_expr
                      << std::endl;
        }
        fail_expr = {};
    }

    std::cout << std::endl << passed << " out of " << tests.size() << " passed"
              << std::endl;

    return tests.size() - passed;
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
    void (_test_##tname)(std::string& fail_expr)

#define EXPECT(cond)                                                           \
    if (!(cond)) {                                                             \
        fail_expr = #cond;                                                     \
        return;                                                                \
    }                                                                          \
    static_assert(true, "") /* require a semicolon after using macro */
