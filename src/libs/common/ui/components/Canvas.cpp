#include "libs/common/ui/components/Canvas.h"
#include "libs/common/ui/core.h"

namespace common::ui::components
{
    Canvas::Canvas() : _drawCallback(nullptr)
    {
    }

    Canvas::Canvas(DrawCallback callback) : _drawCallback(callback)
    {
    }

    Canvas::~Canvas()
    {
    }

    void Canvas::setDrawCallback(DrawCallback callback)
    {
        _drawCallback = callback;
    }

    common::patterns::Result<void> Canvas::render(RenderContext &context)
    {
        if (!_visible)
        {
            return common::patterns::Result<void>::success();
        }

        if (_backgroundColor != Color::Black)
        {
            context.display.fillRect(_x, _y, _width, _height, static_cast<uint16_t>(_backgroundColor));
        }

        if (_drawCallback)
        {
            _drawCallback(context.display, _x, _y, _width, _height);
        }

        return common::patterns::Result<void>::success();
    }

    void Canvas::drawRect(RenderContext &context, int x, int y, int w, int h, Color color)
    {
        context.display.drawRect(_x + x, _y + y, w, h, static_cast<uint16_t>(color));
    }

    void Canvas::fillRect(RenderContext &context, int x, int y, int w, int h, Color color)
    {
        context.display.fillRect(_x + x, _y + y, w, h, static_cast<uint16_t>(color));
    }

    void Canvas::drawCircle(RenderContext &context, int x, int y, int radius, Color color)
    {
        context.display.drawCircle(_x + x, _y + y, radius, static_cast<uint16_t>(color));
    }

    void Canvas::fillCircle(RenderContext &context, int x, int y, int radius, Color color)
    {
        context.display.fillCircle(_x + x, _y + y, radius, static_cast<uint16_t>(color));
    }

    void Canvas::drawLine(RenderContext &context, int x0, int y0, int x1, int y1, Color color)
    {
        context.display.drawLine(_x + x0, _y + y0, _x + x1, _y + y1, static_cast<uint16_t>(color));
    }

    void Canvas::drawTriangle(RenderContext &context, int x0, int y0, int x1, int y1, int x2, int y2, Color color)
    {
        context.display.drawTriangle(_x + x0, _y + y0, _x + x1, _y + y1, _x + x2, _y + y2, static_cast<uint16_t>(color));
    }

    void Canvas::fillTriangle(RenderContext &context, int x0, int y0, int x1, int y1, int x2, int y2, Color color)
    {
        context.display.fillTriangle(_x + x0, _y + y0, _x + x1, _y + y1, _x + x2, _y + y2, static_cast<uint16_t>(color));
    }

    void Canvas::drawRoundRect(RenderContext &context, int x, int y, int w, int h, int radius, Color color)
    {
        context.display.drawRoundRect(_x + x, _y + y, w, h, radius, static_cast<uint16_t>(color));
    }

    void Canvas::fillRoundRect(RenderContext &context, int x, int y, int w, int h, int radius, Color color)
    {
        context.display.fillRoundRect(_x + x, _y + y, w, h, radius, static_cast<uint16_t>(color));
    }

    Screen::Screen() : _backgroundColor(Color::Black)
    {
    }

    Screen::Screen(Color backgroundColor) : _backgroundColor(backgroundColor)
    {
    }

    Screen::~Screen()
    {
    }

    void Screen::setChild(std::unique_ptr<Component> child)
    {
        _child = std::move(child);
    }

    common::patterns::Result<void> Screen::render()
    {
        auto &display = get_display();

        display.fillScreen(static_cast<uint32_t>(static_cast<uint16_t>(_backgroundColor)));

        if (_child && _child->isVisible())
        {
            int screenWidth = get_screen_width();
            int screenHeight = get_screen_height();

            _child->measure(screenWidth, screenHeight);
            _child->layout(0, 0);

            RenderContext context(0, 0, screenWidth, screenHeight, display);
            return _child->render(context);
        }

        return common::patterns::Result<void>::success();
    }

    void Screen::clear()
    {
        auto &display = get_display();
        display.fillScreen(static_cast<uint32_t>(static_cast<uint16_t>(_backgroundColor)));
    }
}
