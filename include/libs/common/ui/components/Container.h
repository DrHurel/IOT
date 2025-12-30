#pragma once
#include "libs/common/ui/components/Component.h"
#include <memory>

namespace common::ui::components
{
    class Container : public Component
    {
    private:
        int _margin;
        int _padding;
        std::unique_ptr<Component> _child;
        bool _hasBorder;
        Color _borderColor;

    public:
        Container();
        explicit Container(int margin, int padding = 0);
        ~Container();

        Container(const Container &) = delete;
        Container &operator=(const Container &) = delete;

        Container(Container &&) = delete;
        Container &operator=(Container &&) = delete;

        void setChild(std::unique_ptr<Component> child);

        void setBorder(bool hasBorder, Color borderColor = Color::White);

        void measure(int availableWidth, int availableHeight) override;
        void layout(int x, int y) override;
        common::patterns::Result<void> render(RenderContext &context) override;

        int getMargin() const { return _margin; }
        int getPadding() const { return _padding; }
    };
}
