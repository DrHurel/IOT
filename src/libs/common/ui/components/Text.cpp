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
        auto &display = get_display();
        display.setTextSize(_fontSize);

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

        display.setTextSize(_fontSize);
        display.setTextColor(static_cast<uint16_t>(_foregroundColor), static_cast<uint16_t>(_backgroundColor));

        int textX = _x;
        int textY = _y;

        switch (_align)
        {
        case Align::LEFT:
            display.setCursor(textX, textY);
            break;

        case Align::CENTER:
        {
            int charWidth = 6 * _fontSize;
            int textWidth = _text.length() * charWidth;
            textX = _x + (_width - textWidth) / 2;
            if (textX < _x)
                textX = _x;
            display.setCursor(textX, textY);
        }
        break;

        case Align::RIGHT:
        {
            int charWidth = 6 * _fontSize;
            int textWidth = _text.length() * charWidth;
            textX = _x + _width - textWidth;
            if (textX < _x)
                textX = _x;
            display.setCursor(textX, textY);
        }
        break;
        }

        display.print(_text.c_str());

        if (_underline)
        {
            int charHeight = 8 * _fontSize;
            int underlineY = textY + charHeight;
            display.drawLine(textX, underlineY, textX + _width, underlineY, static_cast<uint32_t>(_foregroundColor));
        }

        return common::patterns::Result<void>::success();
    }
}
