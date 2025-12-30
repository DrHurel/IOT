#pragma once
#include "libs/common/ui/components/Component.h"
#include <memory>
#include <functional>

namespace common::ui::components
{
    class Canvas : public Component
    {
    public:
        using DrawCallback = std::function<void(TFT_eSPI &, int x, int y, int width, int height)>;

    private:
        DrawCallback _drawCallback;

    public:
        Canvas();
        explicit Canvas(DrawCallback callback);
        ~Canvas();

        Canvas(const Canvas &) = delete;
        Canvas &operator=(const Canvas &) = delete;

        Canvas(Canvas &&) = delete;
        Canvas &operator=(Canvas &&) = delete;

        void setDrawCallback(DrawCallback callback);

        common::patterns::Result<void> render(RenderContext &context) override;

        void drawRect(RenderContext &context, int x, int y, int w, int h, Color color);
        void fillRect(RenderContext &context, int x, int y, int w, int h, Color color);
        void drawCircle(RenderContext &context, int x, int y, int radius, Color color);
        void fillCircle(RenderContext &context, int x, int y, int radius, Color color);
        void drawLine(RenderContext &context, int x0, int y0, int x1, int y1, Color color);
        void drawTriangle(RenderContext &context, int x0, int y0, int x1, int y1, int x2, int y2, Color color);
        void fillTriangle(RenderContext &context, int x0, int y0, int x1, int y1, int x2, int y2, Color color);
        void drawRoundRect(RenderContext &context, int x, int y, int w, int h, int radius, Color color);
        void fillRoundRect(RenderContext &context, int x, int y, int w, int h, int radius, Color color);
    };

    class Screen
    {
    private:
        std::unique_ptr<Component> _child;
        Color _backgroundColor;

    public:
        Screen();
        explicit Screen(Color backgroundColor);
        ~Screen();

        Screen(const Screen &) = delete;
        Screen &operator=(const Screen &) = delete;

        Screen(Screen &&) = delete;
        Screen &operator=(Screen &&) = delete;

        void setChild(std::unique_ptr<Component> child);

        common::patterns::Result<void> render();

        void clear();
    };
}
