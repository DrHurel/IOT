#pragma once
#include <memory>
#include <vector>
#include "libs/common/ui/components/Component.h"
#include "libs/common/ui/components/Column.h"

namespace common::ui::components
{
    class Horizontal : public Component
    {
    private:
        std::vector<std::unique_ptr<Component>> _children;
        MainAxisAlignment _mainAxisAlignment;
        CrossAxisAlignment _crossAxisAlignment;
        int _spacing;

    public:
        explicit Horizontal(MainAxisAlignment mainAlign = MainAxisAlignment::START,
                   CrossAxisAlignment crossAlign = CrossAxisAlignment::START,
                   int spacing = 0);
        ~Horizontal();

        Horizontal(const Horizontal &) = delete;
        Horizontal &operator=(const Horizontal &) = delete;

        Horizontal(Horizontal &&) = delete;
        Horizontal &operator=(Horizontal &&) = delete;

        void addChild(std::unique_ptr<Component> child);

        void measure(int availableWidth, int availableHeight) override;
        void layout(int x, int y) override;
        common::patterns::Result<void> render(RenderContext &context) override;

        void clearChildren();

        size_t getChildCount() const { return _children.size(); }
    };
}