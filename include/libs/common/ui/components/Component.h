#pragma once
#include "libs/common/patterns/Result.h"
#include "libs/common/ui/core.h"

namespace common::ui::components
{
    class Component
    {
    protected:
        int _x;
        int _y;
        int _width;
        int _height;
        Color _backgroundColor;
        Color _foregroundColor;
        bool _visible;

    public:
        Component();
        virtual ~Component();

        Component(const Component &) = delete;
        Component &operator=(const Component &) = delete;

        Component(Component &&) = delete;
        Component &operator=(Component &&) = delete;

        virtual common::patterns::Result<void> render(RenderContext &context) = 0;

        virtual void measure(int availableWidth, int availableHeight);

        virtual void layout(int x, int y);

        int getX() const;
        int getY() const;
        int getWidth() const;
        int getHeight() const;
        bool isVisible() const;

        void setVisible(bool visible);
        void setBackgroundColor(Color color);
        void setForegroundColor(Color color);
        Color getBackgroundColor() const;
        Color getForegroundColor() const;
    };
}
