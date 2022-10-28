#include <algorithm>

#include "test.hpp"
#include "net/network.test.cpp"

using namespace alewa::test;

static size_t passed = 0;

void run_test(TestCase const & test)
{
    std::string fail_expr{};
    test.run(fail_expr);

    if (!fail_expr.empty()) {
        std::cout << "[ FAIL ] " << test.name << ": " << fail_expr << std::endl;
        return;
    }
    ++passed;
    std::cout << "[ PASS ] " << test.name << std::endl;
}

int main()
{
    std::for_each(tests.begin(), tests.end(), run_test);
    std::cout << std::endl << passed << " out of " << tests.size() << " passed"
              << std::endl;

    size_t nfails = tests.size() - passed;
    return (nfails == 0) ? 0 : 1;
}
