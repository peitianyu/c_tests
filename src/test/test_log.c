#include "common/c_test.h"
#include "common/logger.h"


// JUST_RUN_TEST(hello, test)
TEST(hello, test)
{
    LOG("hello world %d %d %d", 1, 2, 3);
}