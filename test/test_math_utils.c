
#include "unity.h"

#include "math_utils.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_add_should_return_sum_of_two_numbers(void) {
    int result = add(2, 3);
    TEST_ASSERT_EQUAL_INT(5, result);
}

void test_add_should_handle_negative_numbers(void) {
    TEST_ASSERT_EQUAL_INT(-1, add(2, -3));
    TEST_ASSERT_EQUAL_INT(0, add(-5, 5));
}
