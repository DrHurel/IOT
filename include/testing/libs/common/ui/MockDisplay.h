#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "TFT_eSPI.h"
#include "libs/common/ui/core.h"

namespace testing::mocks
{
    struct DrawOperation
    {
        enum Type
        {
            FILL_RECT,
            DRAW_TEXT,
            DRAW_PIXEL,
            SET_TEXT_COLOR,
            SET_TEXT_SIZE,
            CLEAR,
            DRAW_LINE,
            DRAW_CIRCLE
        };

        Type type;
        int x;
        int y;
        int width;
        int height;
        std::string text;
        uint16_t color;
        int param1; // For additional parameters like text size, radius, etc.

        DrawOperation()
            : type(CLEAR), x(0), y(0), width(0), height(0), text(""), color(0), param1(0) {}

        DrawOperation(Type t, int x_, int y_, int w_, int h_, const std::string &txt, uint16_t col, int p1 = 0)
            : type(t), x(x_), y(y_), width(w_), height(h_), text(txt), color(col), param1(p1) {}
    };

    class MockDisplay : public TFT_eSPI
    {
    private:
        std::vector<DrawOperation> operations;
        std::vector<uint16_t> pixels; // RGB565 pixel buffer
        int screen_width;
        int screen_height;
        uint16_t current_text_color;
        uint16_t current_text_bg_color;
        int current_text_size;
        int cursor_x;
        int cursor_y;

        // Helper methods for pixel rendering
        void setPixel(int x, int y, uint16_t color);
        void drawLineInternal(int x0, int y0, int x1, int y1, uint16_t color);
        void drawCircleInternal(int xc, int yc, int radius, uint16_t color, bool fill);
        void drawCharacter(char c, int x, int y, uint16_t color, int size);

    public:
        MockDisplay(int width = 135, int height = 240);

        // Override TFT_eSPI methods to capture operations and render pixels
        void fillRect(int x, int y, int w, int h, uint16_t color);
        void drawString(const std::string &text, int x, int y);
        void drawString(const char *text, int x, int y);
        void drawPixel(int x, int y, uint16_t color);
        void fillScreen(uint16_t color);
        void setTextColor(uint16_t color);
        void setTextColor(uint16_t fg, uint16_t bg);
        // Add Color enum overloads
        void setTextColor(common::ui::Color color)
        {
            setTextColor(static_cast<uint16_t>(color));
        }
        void setTextColor(common::ui::Color fg, common::ui::Color bg)
        {
            setTextColor(static_cast<uint16_t>(fg), static_cast<uint16_t>(bg));
        }
        void setTextSize(int size);
        void drawLine(int x0, int y0, int x1, int y1, uint16_t color);
        void drawCircle(int x, int y, int radius, uint16_t color);
        void fillCircle(int x, int y, int radius, uint16_t color);
        void setCursor(int x, int y);
        void print(const char *text);
        void print(const std::string &text);

        // Display properties
        int width() const { return screen_width; }
        int height() const { return screen_height; }

        // Snapshot methods
        std::string generateSnapshot() const;
        bool compareSnapshot(const std::string &expected) const;
        void saveSnapshot(const std::string &filename) const;
        std::string loadSnapshot(const std::string &filename) const;
        void savePNG(const std::string &filename) const;
        bool comparePNG(const std::string &filename) const;
        void clear();

        // Access to operations for testing
        const std::vector<DrawOperation> &getOperations() const { return operations; }
        size_t getOperationCount() const { return operations.size(); }
        const std::vector<uint16_t> &getPixels() const { return pixels; }
    };

} // namespace testing::mocks
