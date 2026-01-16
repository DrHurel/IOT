#pragma once

#include "libs/common/ui/components/Component.h"
#include "libs/common/ui/components/Canvas.h"
#include "libs/common/ui/components/Column.h"
#include "libs/common/ui/components/Horizontal.h"
#include "libs/common/ui/components/Container.h"
#include "libs/common/ui/components/Text.h"
#include "libs/common/ui/core.h"
#include <memory>

namespace common::ui
{
    class CanvasBuilder
    {
    private:
        components::Canvas::DrawCallback _callback;
        Color _bgColor;
        int _width;
        int _height;

    public:
        CanvasBuilder();

        CanvasBuilder &drawCallback(components::Canvas::DrawCallback callback);
        CanvasBuilder &backgroundColor(Color color);
        CanvasBuilder &size(int w, int h);
        std::unique_ptr<components::Canvas> build();
    };

    class TextBuilder
    {
    private:
        std::string _text;
        int _fontSize;
        components::Align _align;
        Color _color;
        bool _underline;

    public:
        explicit TextBuilder(const std::string &text);

        TextBuilder &fontSize(int size);
        TextBuilder &align(components::Align alignment);
        TextBuilder &color(Color c);
        TextBuilder &underline(bool u = true);
        std::unique_ptr<components::Text> build();
    };

    class ContainerBuilder
    {
    private:
        int _margin;
        int _padding;
        Color _bgColor;
        bool _hasBorder;
        Color _borderColor;
        std::unique_ptr<components::Component> _child;

    public:
        ContainerBuilder();

        ContainerBuilder &margin(int m);
        ContainerBuilder &padding(int p);
        ContainerBuilder &backgroundColor(Color c);
        ContainerBuilder &border(bool hasBorder = true, Color color = Color::White);
        ContainerBuilder &child(std::unique_ptr<components::Component> c);
        std::unique_ptr<components::Container> build();
    };

    class ColumnBuilder
    {
    private:
        components::MainAxisAlignment _mainAlign;
        components::CrossAxisAlignment _crossAlign;
        int _spacing;
        Color _bgColor;
        std::vector<std::unique_ptr<components::Component>> _children;

    public:
        ColumnBuilder();

        ColumnBuilder &mainAxisAlignment(components::MainAxisAlignment align);
        ColumnBuilder &crossAxisAlignment(components::CrossAxisAlignment align);
        ColumnBuilder &spacing(int s);
        ColumnBuilder &backgroundColor(Color c);
        ColumnBuilder &addChild(std::unique_ptr<components::Component> child);
        std::unique_ptr<components::Column> build();
    };

    class RowBuilder
    {
    private:
        components::MainAxisAlignment _mainAlign;
        components::CrossAxisAlignment _crossAlign;
        int _spacing;
        Color _bgColor;
        std::vector<std::unique_ptr<components::Component>> _children;

    public:
        RowBuilder();

        RowBuilder &mainAxisAlignment(components::MainAxisAlignment align);
        RowBuilder &crossAxisAlignment(components::CrossAxisAlignment align);
        RowBuilder &spacing(int s);
        RowBuilder &backgroundColor(Color c);
        RowBuilder &addChild(std::unique_ptr<components::Component> child);
        std::unique_ptr<components::Horizontal> build();
    };
}
