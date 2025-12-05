#include "libs/common/ui/core.h"

#ifdef NATIVE_TEST
// For native tests, use the mock implementation
#include "testing/libs/common/ui/MockDisplay.h"
#include <memory>

// Static fallback display for tests that don't provide their own
static testing::mocks::MockDisplay *fallback_display = nullptr;

// Function to get the test display - tests can override this by defining display
__attribute__((weak)) testing::mocks::MockDisplay *get_test_display_ptr()
{
    // This function will be overridden by tests that define their own display management
    return nullptr;
}

namespace common::ui
{
    RenderContext::RenderContext(int x, int y, int w, int h, TFT_eSPI &disp)
        : x(x), y(y), width(w), height(h), display(disp)
    {
    }

    void bootstrap()
    {
        // For tests that don't set up a display, create a fallback
        auto *test_display = get_test_display_ptr();
        if (!test_display && !fallback_display)
        {
            fallback_display = new testing::mocks::MockDisplay();
        }
    }

    int get_screen_width()
    {
        auto *test_display = get_test_display_ptr();
        if (test_display)
        {
            return test_display->width();
        }
        if (fallback_display)
        {
            return fallback_display->width();
        }
        return 135; // Default for LilyGo T-Display
    }

    int get_screen_height()
    {
        auto *test_display = get_test_display_ptr();
        if (test_display)
        {
            return test_display->height();
        }
        if (fallback_display)
        {
            return fallback_display->height();
        }
        return 240; // Default for LilyGo T-Display
    }

    TFT_eSPI &get_display()
    {
        auto *test_display = get_test_display_ptr();
        if (test_display)
        {
            return *reinterpret_cast<TFT_eSPI *>(test_display);
        }
        if (fallback_display)
        {
            return *reinterpret_cast<TFT_eSPI *>(fallback_display);
        }
        // Last resort: create fallback if it doesn't exist
        fallback_display = new testing::mocks::MockDisplay();
        return *reinterpret_cast<TFT_eSPI *>(fallback_display);
    }
}

#else
// For real hardware, use the actual TFT_eSPI implementation
#include <TFT_eSPI.h>

namespace common::ui
{
    static TFT_eSPI tft = TFT_eSPI();
    static bool initialized = false;

    RenderContext::RenderContext(int x, int y, int w, int h, TFT_eSPI &disp)
        : x(x), y(y), width(w), height(h), display(disp)
    {
    }

    void bootstrap()
    {
        if (!initialized)
        {
            tft.init();
            tft.setRotation(1); // Landscape mode for LilyGo T-Display
            tft.fillScreen(static_cast<uint16_t>(Color::Black));
            initialized = true;
        }
    }

    int get_screen_width()
    {
        return tft.width();
    }

    int get_screen_height()
    {
        return tft.height();
    }

    TFT_eSPI &get_display()
    {
        return tft;
    }
}
#endif
