#include "libs/common/ui/components/Component.h"

namespace common::ui::components
{
    Component::Component()
        : _x(0), _y(0), _width(0), _height(0),
          _backgroundColor(Color::Black), _foregroundColor(Color::White), _visible(true)
    {
    }

    Component::~Component()
    {
    }

    void Component::measure(int availableWidth, int availableHeight)
    {
        _width = availableWidth;
        _height = availableHeight;
    }

    void Component::layout(int x, int y)
    {
        _x = x;
        _y = y;
    }

    int Component::getX() const
    {
        return _x;
    }

    int Component::getY() const
    {
        return _y;
    }

    int Component::getWidth() const
    {
        return _width;
    }

    int Component::getHeight() const
    {
        return _height;
    }

    bool Component::isVisible() const
    {
        return _visible;
    }

    void Component::setVisible(bool visible)
    {
        _visible = visible;
    }

    void Component::setBackgroundColor(Color color)
    {
        _backgroundColor = color;
    }

    void Component::setForegroundColor(Color color)
    {
        _foregroundColor = color;
    }

    Color Component::getBackgroundColor() const
    {
        return _backgroundColor;
    }

    Color Component::getForegroundColor() const
    {
        return _foregroundColor;
    }
}
