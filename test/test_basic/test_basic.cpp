#include <unity.h>

void setUp(void) {
    // Setup before each test
}

void tearDown(void) {
    // Cleanup after each test
}

void test_basic_addition() {
    TEST_ASSERT_EQUAL(4, 2 + 2);
}

void test_basic_subtraction() {
    TEST_ASSERT_EQUAL(0, 2 - 2);
}

void test_basic_multiplication() {
    TEST_ASSERT_EQUAL(10, 5 * 2);
}

void test_basic_comparison() {
    TEST_ASSERT_TRUE(5 > 3);
    TEST_ASSERT_FALSE(2 > 5);
}

void test_basic_string() {
    const char* str = "Hello";
    TEST_ASSERT_EQUAL_STRING("Hello", str);
}

void setup() {
    UNITY_BEGIN();
    
    RUN_TEST(test_basic_addition);
    RUN_TEST(test_basic_subtraction);
    RUN_TEST(test_basic_multiplication);
    RUN_TEST(test_basic_comparison);
    RUN_TEST(test_basic_string);
    
    UNITY_END();
}

void loop() {
    // Nothing to do here
}
