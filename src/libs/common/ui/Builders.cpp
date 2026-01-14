#include "libs/common/ui/Builders.h"

namespace common::ui
{
    CanvasBuilder::CanvasBuilder()
        : _callback(nullptr), _bgColor(Color::Black), _width(0), _height(0)
    {
    }

    CanvasBuilder &CanvasBuilder::drawCallback(components::Canvas::DrawCallback callback)
    {
        _callback = callback;
        return *this;
    }

    CanvasBuilder &CanvasBuilder::backgroundColor(Color color)
    {
        _bgColor = color;
        return *this;
    }

    CanvasBuilder &CanvasBuilder::size(int w, int h)
    {
        _width = w;
        _height = h;
        return *this;
    }

    std::unique_ptr<components::Canvas> CanvasBuilder::build()
    {
        auto canvas = std::make_unique<components::Canvas>(_callback);
        canvas->setBackgroundColor(_bgColor);
        return canvas;
    }

    TextBuilder::TextBuilder(const std::string &text)
        : _text(text), _fontSize(2), _align(components::Align::LEFT),
          _color(Color::White), _underline(false)
    {
    }

    TextBuilder &TextBuilder::fontSize(int size)
    {
        _fontSize = size;
        return *this;
    }

    TextBuilder &TextBuilder::align(components::Align alignment)
    {
        _align = alignment;
        return *this;
    }

    TextBuilder &TextBuilder::color(Color c)
    {
        _color = c;
        return *this;
    }

    TextBuilder &TextBuilder::underline(bool u)
    {
        _underline = u;
        return *this;
    }

    std::unique_ptr<components::Text> TextBuilder::build()
    {
        auto text = std::make_unique<components::Text>(_text, _fontSize, _align, false, false, _underline);
        text->setForegroundColor(_color);
        return text;
    }

    ContainerBuilder::ContainerBuilder()
        : _margin(0), _padding(0), _bgColor(Color::Black),
          _hasBorder(false), _borderColor(Color::White), _child(nullptr)
    {
    }

    ContainerBuilder &ContainerBuilder::margin(int m)
    {
        _margin = m;
        return *this;
    }

    ContainerBuilder &ContainerBuilder::padding(int p)
    {
        _padding = p;
        return *this;
    }

    ContainerBuilder &ContainerBuilder::backgroundColor(Color c)
    {
        _bgColor = c;
        return *this;
    }

    ContainerBuilder &ContainerBuilder::border(bool hasBorder, Color color)
    {
        _hasBorder = hasBorder;
        _borderColor = color;
        return *this;
    }

    ContainerBuilder &ContainerBuilder::child(std::unique_ptr<components::Component> c)
    {
        _child = std::move(c);
        return *this;
    }

    std::unique_ptr<components::Container> ContainerBuilder::build()
    {
        auto container = std::make_unique<components::Container>(_margin, _padding);
        container->setBackgroundColor(_bgColor);
        container->setBorder(_hasBorder, _borderColor);
        if (_child)
        {
            container->setChild(std::move(_child));
        }
        return container;
    }

    ColumnBuilder::ColumnBuilder()
        : _mainAlign(components::MainAxisAlignment::START),
          _crossAlign(components::CrossAxisAlignment::START),
          _spacing(0), _bgColor(Color::Black)
    {
    }

    ColumnBuilder &ColumnBuilder::mainAxisAlignment(components::MainAxisAlignment align)
    {
        _mainAlign = align;
        return *this;
    }

    ColumnBuilder &ColumnBuilder::crossAxisAlignment(components::CrossAxisAlignment align)
    {
        _crossAlign = align;
        return *this;
    }

    ColumnBuilder &ColumnBuilder::spacing(int s)
    {
        _spacing = s;
        return *this;
    }

    ColumnBuilder &ColumnBuilder::backgroundColor(Color c)
    {
        _bgColor = c;
        return *this;
    }

    ColumnBuilder &ColumnBuilder::addChild(std::unique_ptr<components::Component> child)
    {
        _children.push_back(std::move(child));
        return *this;
    }

    std::unique_ptr<components::Column> ColumnBuilder::build()
    {
        auto column = std::make_unique<components::Column>(_mainAlign, _crossAlign, _spacing);
        column->setBackgroundColor(_bgColor);
        for (auto &child : _children)
        {
            column->addChild(std::move(child));
        }
        return column;
    }

    RowBuilder::RowBuilder()
        : _mainAlign(components::MainAxisAlignment::START),
          _crossAlign(components::CrossAxisAlignment::START),
          _spacing(0), _bgColor(Color::Black)
    {
    }

    RowBuilder &RowBuilder::mainAxisAlignment(components::MainAxisAlignment align)
    {
        _mainAlign = align;
        return *this;
    }

    RowBuilder &RowBuilder::crossAxisAlignment(components::CrossAxisAlignment align)
    {
        _crossAlign = align;
        return *this;
    }

    RowBuilder &RowBuilder::spacing(int s)
    {
        _spacing = s;
        return *this;
    }

    RowBuilder &RowBuilder::backgroundColor(Color c)
    {
        _bgColor = c;
        return *this;
    }

    RowBuilder &RowBuilder::addChild(std::unique_ptr<components::Component> child)
    {
        _children.push_back(std::move(child));
        return *this;
    }

    std::unique_ptr<components::Horizontal> RowBuilder::build()
    {
        auto row = std::make_unique<components::Horizontal>(_mainAlign, _crossAlign, _spacing);
        row->setBackgroundColor(_bgColor);
        for (auto &child : _children)
        {
            row->addChild(std::move(child));
        }
        return row;
    }
}
