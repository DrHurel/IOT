#include "libs/common/ui/components/Text.h"
#include "libs/common/ui/core.h"

namespace common::ui::components
{
    Text::Text(const std::string &text, int fontSize, Align align, bool bold, bool italic, bool underline)
        : _text(text), _fontSize(fontSize), _align(align), _bold(bold), _italic(italic), _underline(underline)
    {
    }

    Text::~Text() = default;

    void Text::setText(const std::string &text)
    {
        _text = text;
    }

    void Text::measure(int availableWidth, int availableHeight)
    {
        // Estimate text dimensions based on font size
        // TFT_eSPI fonts have different sizes, we approximate here
        auto &display = get_display();
        display.setTextSize(_fontSize);

        // Get text bounds (approximate)
        int16_t x1, y1;
        uint16_t w, h;
        display.setTextSize(_fontSize);

        // Approximate character dimensions
        // This is a rough estimate - actual size depends on font
        int charWidth = 6 * _fontSize;
        int charHeight = 8 * _fontSize;

        _width = std::min(static_cast<int>(_text.length() * charWidth), availableWidth);
        _height = std::min(charHeight, availableHeight);
    }

    common::patterns::Result<void> Text::render(RenderContext &context)
    {
        if (!_visible || _text.empty())
        {
            return common::patterns::Result<void>::success();
        }

        auto &display = context.display;

        // Set text properties
        display.setTextSize(_fontSize);
        display.setTextColor(_foregroundColor, _backgroundColor);

        // Calculate text position based on alignment
        int textX = _x;
        int textY = _y;

        // Set cursor and draw
        switch (_align)
        {
        case Align::LEFT:
            display.setCursor(textX, textY);
            break;

        case Align::CENTER:
        {
            // Approximate center alignment
            int charWidth = 6 * _fontSize;
            int textWidth = _text.length() * charWidth;
            textX = _x + (_width - textWidth) / 2;
            if (textX < _x)
                textX = _x; // Ensure we don't go negative
            display.setCursor(textX, textY);
        }
        break;

        case Align::RIGHT:
        {
            // Approximate right alignment
            int charWidth = 6 * _fontSize;
            int textWidth = _text.length() * charWidth;
            textX = _x + _width - textWidth;
            if (textX < _x)
                textX = _x; // Ensure we don't go negative
            display.setCursor(textX, textY);
        }
        break;
        }

        // Draw the text
        display.print(_text.c_str());

        // Draw underline if requested
        if (_underline)
        {
            int charHeight = 8 * _fontSize;
            int underlineY = textY + charHeight;
            display.drawLine(textX, underlineY, textX + _width, underlineY, _foregroundColor);
        }

        return common::patterns::Result<void>::success();
    }
}
