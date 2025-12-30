#pragma once

#include "libs/common/patterns/Result.h"

#ifdef NATIVE_TEST
#include "testing/libs/common/ui/TFT_eSPI.h"
#else
#include <TFT_eSPI.h>
#endif

#include <memory>
#include <vector>

namespace common::ui
{
    class RenderContext;

    void bootstrap();

    int get_screen_width();
    int get_screen_height();

    TFT_eSPI &get_display();

    enum class Color : uint16_t
    {
        Black = 0x0000,
        White = 0xFFFF,
        Red = 0xF800,
        Green = 0x07E0,
        Blue = 0x001F,
        Yellow = 0xFFE0,
        Cyan = 0x07FF,
        Magenta = 0xF81F,
        Gray = 0x8410,
        DarkGray = 0x4208,
        LightGray = 0xC618
    };

    struct RenderContext
    {
        int x;
        int y;
        int width;
        int height;
        TFT_eSPI &display;

        RenderContext(int x, int y, int w, int h, TFT_eSPI &disp);
    };
}
