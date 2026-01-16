#pragma once
#include "libs/common/ui/components/Component.h"
#include <string>

namespace common::ui::components
{
    enum class Align
    {
        CENTER,
        LEFT,
        RIGHT
    };

    class Text : public Component
    {
    private:
        std::string _text;
        int _fontSize;
        Align _align;
        bool _bold;
        bool _italic;
        bool _underline;

    public:
        explicit Text(const std::string &text, int fontSize = 2, Align align = Align::LEFT,
             bool bold = false, bool italic = false, bool underline = false);
        ~Text();

        Text(const Text &) = delete;
        Text &operator=(const Text &) = delete;

        Text(Text &&) = delete;
        Text &operator=(Text &&) = delete;

        void setText(const std::string &text);

        const std::string &getText() const { return _text; }

        void measure(int availableWidth, int availableHeight) override;
        common::patterns::Result<void> render(RenderContext &context) override;

        void setFontSize(int fontSize) { _fontSize = fontSize; }
        void setAlign(Align align) { _align = align; }
    };
}
