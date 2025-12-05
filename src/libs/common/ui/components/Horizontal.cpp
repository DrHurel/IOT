#include "libs/common/ui/components/Horizontal.h"
#include "libs/common/ui/core.h"
#include <algorithm>

namespace common::ui::components
{
    Horizontal::Horizontal(MainAxisAlignment mainAlign, CrossAxisAlignment crossAlign, int spacing)
        : _mainAxisAlignment(mainAlign), _crossAxisAlignment(crossAlign), _spacing(spacing)
    {
    }

    Horizontal::~Horizontal() = default;

    void Horizontal::addChild(std::unique_ptr<Component> child)
    {
        _children.push_back(std::move(child));
    }

    void Horizontal::clearChildren()
    {
        _children.clear();
    }

    void Horizontal::measure(int availableWidth, int availableHeight)
    {
        _width = availableWidth;
        _height = availableHeight;

        // Measure all children
        int totalWidth = 0;
        int maxHeight = 0;

        for (auto &child : _children)
        {
            if (child && child->isVisible())
            {
                child->measure(availableWidth - totalWidth, availableHeight);
                totalWidth += child->getWidth();
                maxHeight = std::max(maxHeight, child->getHeight());
            }
        }

        // Add spacing between children
        if (_children.size() > 1)
        {
            totalWidth += _spacing * (_children.size() - 1);
        }

        // Horizontal takes the minimum of available width and total children width
        _width = std::min(totalWidth, availableWidth);
    }

    void Horizontal::layout(int x, int y)
    {
        _x = x;
        _y = y;

        if (_children.empty())
        {
            return;
        }

        // Calculate total width of visible children
        int totalChildWidth = 0;
        int visibleCount = 0;
        for (auto &child : _children)
        {
            if (child && child->isVisible())
            {
                totalChildWidth += child->getWidth();
                visibleCount++;
            }
        }

        // Add spacing
        if (visibleCount > 1)
        {
            totalChildWidth += _spacing * (visibleCount - 1);
        }

        // Calculate starting X position based on main axis alignment
        int currentX = x;
        int extraSpace = _width - totalChildWidth;

        switch (_mainAxisAlignment)
        {
        case MainAxisAlignment::START:
            currentX = x;
            break;
        case MainAxisAlignment::END:
            currentX = x + extraSpace;
            break;
        case MainAxisAlignment::CENTER:
            currentX = x + extraSpace / 2;
            break;
        case MainAxisAlignment::SPACE_BETWEEN:
            currentX = x;
            if (visibleCount > 1)
            {
                _spacing = extraSpace / (visibleCount - 1);
            }
            break;
        case MainAxisAlignment::SPACE_AROUND:
            if (visibleCount > 0)
            {
                int spacing = extraSpace / (visibleCount * 2);
                currentX = x + spacing;
                _spacing = spacing * 2;
            }
            break;
        case MainAxisAlignment::SPACE_EVENLY:
            if (visibleCount > 0)
            {
                int spacing = extraSpace / (visibleCount + 1);
                currentX = x + spacing;
                _spacing = spacing;
            }
            break;
        }

        // Layout each child
        for (auto &child : _children)
        {
            if (child && child->isVisible())
            {
                int childY = y;
                int childHeight = child->getHeight();

                // Apply cross axis alignment
                switch (_crossAxisAlignment)
                {
                case CrossAxisAlignment::START:
                    childY = y;
                    break;
                case CrossAxisAlignment::END:
                    childY = y + _height - childHeight;
                    break;
                case CrossAxisAlignment::CENTER:
                    childY = y + (_height - childHeight) / 2;
                    break;
                case CrossAxisAlignment::STRETCH:
                    childY = y;
                    child->measure(child->getWidth(), _height);
                    break;
                }

                child->layout(currentX, childY);
                currentX += child->getWidth() + _spacing;
            }
        }
    }

    common::patterns::Result<void> Horizontal::render(RenderContext &context)
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
