#pragma once

// Mock TFT_eSPI for native testing
// This file provides a minimal TFT_eSPI interface for compilation on native platform

#ifdef NATIVE_TEST

#include <cstdint>
#include <string>

// Forward declare Color enum from common::ui namespace
namespace common::ui
{
    enum class Color : uint16_t;
}

// Mock TFT_eSPI class - will be replaced with MockDisplay in tests
class TFT_eSPI
{
private:
    int _width;
    int _height;
    int _rotation;
    uint16_t _textColor;
    uint16_t _textBgColor;
    int _textSize;

public:
    TFT_eSPI() : _width(135), _height(240), _rotation(0), _textColor(0xFFFF), _textBgColor(0x0000), _textSize(1) {}
    virtual ~TFT_eSPI() {}

    // Initialization
    virtual void init() {}
    virtual void setRotation(int rotation) { _rotation = rotation; }

    // Display dimensions
    virtual int width() const { return _width; }
    virtual int height() const { return _height; }

    // Screen operations
    virtual void fillScreen(uint16_t color) {}
    virtual void fillScreen(uint32_t color) {}

    // Drawing primitives
    virtual void fillRect(int x, int y, int w, int h, uint16_t color) {}
    virtual void drawRect(int x, int y, int w, int h, uint16_t color) {}
    virtual void drawLine(int x0, int y0, int x1, int y1, uint16_t color) {}
    virtual void drawPixel(int x, int y, uint16_t color) {}

    // Overloads accepting common::ui::Color enum
    void fillRect(int x, int y, int w, int h, common::ui::Color color)
    {
        fillRect(x, y, w, h, static_cast<uint16_t>(color));
    }
    void drawRect(int x, int y, int w, int h, common::ui::Color color)
    {
        drawRect(x, y, w, h, static_cast<uint16_t>(color));
    }
    void drawLine(int x0, int y0, int x1, int y1, common::ui::Color color)
    {
        drawLine(x0, y0, x1, y1, static_cast<uint16_t>(color));
    }
    void drawPixel(int x, int y, common::ui::Color color)
    {
        drawPixel(x, y, static_cast<uint16_t>(color));
    }

    // Circle operations
    virtual void drawCircle(int x, int y, int radius, uint16_t color) {}
    virtual void fillCircle(int x, int y, int radius, uint16_t color) {}

    void drawCircle(int x, int y, int radius, common::ui::Color color)
    {
        drawCircle(x, y, radius, static_cast<uint16_t>(color));
    }
    void fillCircle(int x, int y, int radius, common::ui::Color color)
    {
        fillCircle(x, y, radius, static_cast<uint16_t>(color));
    }

    // Triangle operations
    virtual void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {}
    virtual void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint16_t color) {}

    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, common::ui::Color color)
    {
        drawTriangle(x0, y0, x1, y1, x2, y2, static_cast<uint16_t>(color));
    }
    void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, common::ui::Color color)
    {
        fillTriangle(x0, y0, x1, y1, x2, y2, static_cast<uint16_t>(color));
    }

    // Rounded rectangle operations
    void drawRoundRect(int x, int y, int w, int h, int radius, uint16_t color) {}
    void fillRoundRect(int x, int y, int w, int h, int radius, uint16_t color) {}

    void drawRoundRect(int x, int y, int w, int h, int radius, common::ui::Color color)
    {
        drawRoundRect(x, y, w, h, radius, static_cast<uint16_t>(color));
    }
    void fillRoundRect(int x, int y, int w, int h, int radius, common::ui::Color color)
    {
        fillRoundRect(x, y, w, h, radius, static_cast<uint16_t>(color));
    }

    // Text operations
    virtual void setTextColor(uint16_t color) { _textColor = color; }
    virtual void setTextColor(uint16_t fg, uint16_t bg)
    {
        _textColor = fg;
        _textBgColor = bg;
    }
    void setTextColor(common::ui::Color color)
    {
        setTextColor(static_cast<uint16_t>(color)); // Call virtual method
    }
    void setTextColor(common::ui::Color fg, common::ui::Color bg)
    {
        setTextColor(static_cast<uint16_t>(fg), static_cast<uint16_t>(bg)); // Call virtual method
    }
    virtual void setTextSize(int size) { _textSize = size; }
    virtual void drawString(const char *text, int x, int y) {}
    virtual void drawString(const std::string &text, int x, int y) {}
    virtual void setCursor(int x, int y) {}
    virtual void print(const char *text) {}
    virtual void print(const std::string &text) {}

    // Text measurement (approximate)
    virtual int textWidth(const char *text) { return std::string_view(text).length() * 6 * _textSize; }
    virtual int textWidth(const std::string &text) { return text.length() * 6 * _textSize; }
    virtual int fontHeight() { return 8 * _textSize; }
};

#endif // NATIVE_TEST
