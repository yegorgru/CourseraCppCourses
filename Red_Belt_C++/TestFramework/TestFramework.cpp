#include "Test.h"

TestRunner::~TestRunner() {
    if (fail_count > 0) {
        std::cerr << fail_count << " unit tests failed. Terminate" << std::endl;
        exit(1);
    }
}

inline void Assert(bool b, const std::string& hint) {
    AssertEqual(b, true, hint);
}

void func() {
    int x = 5;
    ASSERT_EQUAL(x, 4);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, func);
}