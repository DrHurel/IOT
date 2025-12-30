#include "testing/ui/helpers.h"

// ============================================================================
// Mock Display Tests
// ============================================================================

void test_mock_display_basic_operations()
{
    display->fillScreen(0x0000);
    display->setTextColor(0xFFFF);
    display->setTextSize(2);
    display->drawString("Hello", 10, 20);
    display->fillRect(50, 50, 100, 80, 0xF800);

    TEST_ASSERT_EQUAL(5, display->getOperationCount());
    TEST_ASSERT_TRUE(assertSnapshot("mock_display_basic"));
}

void test_mock_display_multiple_shapes()
{
    display->fillScreen(0x0000);
    display->drawLine(0, 0, 100, 100, 0xFFFF);
    display->drawCircle(50, 50, 20, 0x07E0);
    display->fillCircle(100, 100, 15, 0xF800);

    TEST_ASSERT_TRUE(assertSnapshot("mock_display_shapes"));
}
