#include "common/c_test.h"
#include "common/logger.h"
#include "core/oop.h"

JUST_RUN_TEST(oop, test)
TEST(oop, test)
{
    LOG_DEBUG("--------------------oop test---------------");

    // 不具备派生能力, 仅支持单次使用
    triangle_init(1.0, 1.0);

    LOG_DEBUG("triangle area: %f", get_area());
}