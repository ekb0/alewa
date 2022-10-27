#include "test.hpp"

int main() {
    using namespace alewa::test;

    size_t nfails = run_all_tests();
    return (nfails == 0) ? 0 : 1;
}
