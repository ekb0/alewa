#include <algorithm>

#include "test.hpp"
#include "net/socket.test.cpp"

using namespace alewa::test;

static size_t passed = 0;

void run_test(TestCase const & test)
{
    std::string fail_expr{};
    try {
        test.run(fail_expr);
    }
    catch(std::runtime_error const & e) {
        fail_expr = std::string{"unexpected exception: "} + e.what();                                                           \
    }

    if (!fail_expr.empty()) {
        std::cout << "[ FAIL ] " << test.name << ": " << fail_expr << std::endl;
        return;
    }
    ++passed;
    std::cout << "[ PASS ] " << test.name << std::endl;
}

int main()
{
    for (auto const & test : tests) { run_test(test); }
    std::cout << std::endl << passed << " out of " << tests.size() << " passed"
              << std::endl;

    size_t nfails = tests.size() - passed;
    return (nfails == 0) ? 0 : 1;
}
