#include "libs/common/ui/components/Column.h"
#include "libs/common/ui/core.h"
#include <algorithm>

namespace common::ui::components
{
    Column::Column(MainAxisAlignment mainAlign, CrossAxisAlignment crossAlign, int spacing)
        : _mainAxisAlignment(mainAlign), _crossAxisAlignment(crossAlign), _spacing(spacing)
    {
    }

    Column::~Column()
    {
    }

    void Column::addChild(std::unique_ptr<Component> child)
    {
        _children.push_back(std::move(child));
    }

    void Column::clearChildren()
    {
        _children.clear();
    }

    void Column::measure(int availableWidth, int availableHeight)
    {
        _width = availableWidth;
        _height = availableHeight;

        // Measure all children
        int totalHeight = 0;
        int maxWidth = 0;

        for (auto &child : _children)
        {
            if (child && child->isVisible())
            {
                child->measure(availableWidth, availableHeight - totalHeight);
                totalHeight += child->getHeight();
                maxWidth = std::max(maxWidth, child->getWidth());
            }
        }

        // Add spacing between children
        if (_children.size() > 1)
        {
            totalHeight += _spacing * (_children.size() - 1);
        }

        // Column takes the minimum of available height and total children height
        _height = std::min(totalHeight, availableHeight);
    }

    void Column::layout(int x, int y)
    {
        _x = x;
        _y = y;

        if (_children.empty())
        {
            return;
        }

        // Calculate total height of visible children
        int totalChildHeight = 0;
        int visibleCount = 0;
        for (auto &child : _children)
        {
            if (child && child->isVisible())
            {
                totalChildHeight += child->getHeight();
                visibleCount++;
            }
        }

        // Add spacing
        if (visibleCount > 1)
        {
            totalChildHeight += _spacing * (visibleCount - 1);
        }

        // Calculate starting Y position based on main axis alignment
        int currentY = y;
        int extraSpace = _height - totalChildHeight;

        switch (_mainAxisAlignment)
        {
        case MainAxisAlignment::START:
            currentY = y;
            break;
        case MainAxisAlignment::END:
            currentY = y + extraSpace;
            break;
        case MainAxisAlignment::CENTER:
            currentY = y + extraSpace / 2;
            break;
        case MainAxisAlignment::SPACE_BETWEEN:
            currentY = y;
            if (visibleCount > 1)
            {
                _spacing = extraSpace / (visibleCount - 1);
            }
            break;
        case MainAxisAlignment::SPACE_AROUND:
            if (visibleCount > 0)
            {
                int spacing = extraSpace / (visibleCount * 2);
                currentY = y + spacing;
                _spacing = spacing * 2;
            }
            break;
        case MainAxisAlignment::SPACE_EVENLY:
            if (visibleCount > 0)
            {
                int spacing = extraSpace / (visibleCount + 1);
                currentY = y + spacing;
                _spacing = spacing;
            }
            break;
        }

        // Layout each child
        for (auto &child : _children)
        {
            if (child && child->isVisible())
            {
                int childX = x;
                int childWidth = child->getWidth();

                // Apply cross axis alignment
                switch (_crossAxisAlignment)
                {
                case CrossAxisAlignment::START:
                    childX = x;
                    break;
                case CrossAxisAlignment::END:
                    childX = x + _width - childWidth;
                    break;
                case CrossAxisAlignment::CENTER:
                    childX = x + (_width - childWidth) / 2;
                    break;
                case CrossAxisAlignment::STRETCH:
                    childX = x;
                    child->measure(_width, child->getHeight());
                    break;
                }

                child->layout(childX, currentY);
                currentY += child->getHeight() + _spacing;
            }
        }
    }

    common::patterns::Result<void> Column::render(RenderContext &context)
    {
        if (!_visible)
        {
            return common::patterns::Result<void>::success();
        }

        // Render background if set
        if (_backgroundColor != Color::Black)
        {
            context.display.fillRect(_x, _y, _width, _height, static_cast<uint16_t>(_backgroundColor));
        }

        // Render all children
        for (auto &child : _children)
        {
            if (child && child->isVisible())
            {
                auto result = child->render(context);
                if (result.failed())
                {
                    return result;
                }
            }
        }

        return common::patterns::Result<void>::success();
    }
}
