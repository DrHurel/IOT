#pragma once
#include <memory>
#include <vector>
#include "libs/common/ui/components/Component.h"

namespace common::ui::components
{
    enum class MainAxisAlignment
    {
        START,
        END,
        CENTER,
        SPACE_BETWEEN,
        SPACE_AROUND,
        SPACE_EVENLY
    };

    enum class CrossAxisAlignment
    {
        START,
        END,
        CENTER,
        STRETCH
    };

    class Column : public Component
    {
    private:
        std::vector<std::unique_ptr<Component>> _children;
        MainAxisAlignment _mainAxisAlignment;
        CrossAxisAlignment _crossAxisAlignment;
        int _spacing;

    public:
        Column(MainAxisAlignment mainAlign = MainAxisAlignment::START,
               CrossAxisAlignment crossAlign = CrossAxisAlignment::START,
               int spacing = 0);
        ~Column();

        Column(const Column &) = delete;
        Column &operator=(const Column &) = delete;

        Column(Column &&) = delete;
        Column &operator=(Column &&) = delete;

        void addChild(std::unique_ptr<Component> child);

        void measure(int availableWidth, int availableHeight) override;
        void layout(int x, int y) override;
        common::patterns::Result<void> render(RenderContext &context) override;

        void clearChildren();

        size_t getChildCount() const { return _children.size(); }
    };
}
