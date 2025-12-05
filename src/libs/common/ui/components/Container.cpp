#include "libs/common/ui/components/Container.h"
#include "libs/common/ui/core.h"
#include <algorithm>

namespace common::ui::components
{
    Container::Container() : _margin(0), _padding(0), _hasBorder(false), _borderColor(Color::White)
    {
    }

    Container::Container(int margin, int padding)
        : _margin(margin), _padding(padding), _hasBorder(false), _borderColor(Color::White)
    {
    }

    Container::~Container() = default;
    void Container::setChild(std::unique_ptr<Component> child)
    {
        _child = std::move(child);
    }

    void Container::setBorder(bool hasBorder, Color borderColor)
    {
        _hasBorder = hasBorder;
        _borderColor = borderColor;
    }

    void Container::measure(int availableWidth, int availableHeight)
    {
        // Container size includes margin on all sides
        _width = availableWidth;
        _height = availableHeight;

        if (_child)
        {
            // Calculate available space for child (subtract margin, padding, and border)
            int borderWidth = _hasBorder ? 2 : 0;
            int childWidth = std::max(0, availableWidth - 2 * (_margin + _padding + borderWidth));
            int childHeight = std::max(0, availableHeight - 2 * (_margin + _padding + borderWidth));

            _child->measure(childWidth, childHeight);
        }
    }

    void Container::layout(int x, int y)
    {
        _x = x;
        _y = y;

        if (_child)
        {
            // Position child with margin, padding, and border offset
            int borderWidth = _hasBorder ? 1 : 0;
            int childX = x + _margin + _padding + borderWidth;
            int childY = y + _margin + _padding + borderWidth;

            _child->layout(childX, childY);
        }
    }

    common::patterns::Result<void> Container::render(RenderContext &context)
    {
        if (!_visible)
        {
            return common::patterns::Result<void>::success();
        }

        auto &display = context.display;

        // Draw background with margin applied
        int bgX = _x + _margin;
        int bgY = _y + _margin;
        int bgWidth = _width - 2 * _margin;
        int bgHeight = _height - 2 * _margin;

        if (bgWidth > 0 && bgHeight > 0)
        {
            display.fillRect(bgX, bgY, bgWidth, bgHeight, static_cast<uint16_t>(_backgroundColor));

            // Draw border if enabled
            if (_hasBorder)
            {
                display.drawRect(bgX, bgY, bgWidth, bgHeight, static_cast<uint16_t>(_borderColor));
            }
        }

        // Render child if it exists
        if (_child && _child->isVisible())
        {
            int borderWidth = _hasBorder ? 1 : 0;
            int childX = _x + _margin + _padding + borderWidth;
            int childY = _y + _margin + _padding + borderWidth;
            int childWidth = std::max(0, _width - 2 * (_margin + _padding + borderWidth));
            int childHeight = std::max(0, _height - 2 * (_margin + _padding + borderWidth));

            RenderContext childContext(childX, childY, childWidth, childHeight, display);
            return _child->render(childContext);
        }

        return common::patterns::Result<void>::success();
    }
}
