#include "test_ui_helpers.h"
#include "libs/common/ui/Builders.h"
#include "libs/common/ui/components/Text.h"
#include "libs/common/ui/components/Container.h"
#include "libs/common/ui/components/Column.h"
#include "libs/common/ui/components/Canvas.h"

using namespace common::ui::components;

// ============================================================================
// Component Property Tests (no rendering)
// ============================================================================

void test_text_component_simple()
{
    // Create a simple text component
    auto text = TextBuilder("Hello World")
                    .fontSize(2)
                    .color(Color::White)
                    .build();

    // Measure and layout
    text->measure(135, 240);
    text->layout(10, 20);

    // We can test properties without rendering
    TEST_ASSERT_EQUAL_STRING("Hello World", text->getText().c_str());
    TEST_ASSERT_EQUAL(10, text->getX());
    TEST_ASSERT_EQUAL(20, text->getY());
}

void test_text_component_different_colors()
{
    auto redText = TextBuilder("Red Text")
                       .fontSize(3)
                       .color(Color::Red)
                       .build();

    auto blueText = TextBuilder("Blue Text")
                        .fontSize(2)
                        .color(Color::Blue)
                        .build();

    redText->measure(135, 240);
    redText->layout(0, 0);

    blueText->measure(135, 240);
    blueText->layout(0, 30);

    TEST_ASSERT_EQUAL(static_cast<uint16_t>(Color::Red),
                      static_cast<uint16_t>(redText->getForegroundColor()));
    TEST_ASSERT_EQUAL(static_cast<uint16_t>(Color::Blue),
                      static_cast<uint16_t>(blueText->getForegroundColor()));
}

void test_container_with_padding()
{
    auto text = TextBuilder("Padded").build();

    auto container = ContainerBuilder()
                         .padding(10)
                         .margin(5)
                         .backgroundColor(Color::Blue)
                         .child(std::move(text))
                         .build();

    container->measure(135, 240);
    container->layout(0, 0);

    TEST_ASSERT_EQUAL(10, container->getPadding());
    TEST_ASSERT_EQUAL(5, container->getMargin());
}

void test_container_with_border()
{
    auto text = TextBuilder("Bordered").build();

    auto container = ContainerBuilder()
                         .padding(5)
                         .backgroundColor(Color::Black)
                         .border(true, Color::White)
                         .child(std::move(text))
                         .build();

    container->measure(135, 240);
    container->layout(10, 10);

    TEST_ASSERT_TRUE(container->isVisible());
}

void test_column_layout_multiple_children()
{
    auto column = ColumnBuilder()
                      .mainAxisAlignment(MainAxisAlignment::CENTER)
                      .crossAxisAlignment(CrossAxisAlignment::CENTER)
                      .spacing(5)
                      .backgroundColor(Color::Black)
                      .addChild(TextBuilder("Line 1").build())
                      .addChild(TextBuilder("Line 2").build())
                      .addChild(TextBuilder("Line 3").build())
                      .build();

    column->measure(135, 240);
    column->layout(0, 0);

    TEST_ASSERT_TRUE(column->isVisible());
}

void test_nested_containers()
{
    auto innerText = TextBuilder("Inner").fontSize(2).build();

    auto innerContainer = ContainerBuilder()
                              .padding(5)
                              .backgroundColor(Color::Blue)
                              .child(std::move(innerText))
                              .build();

    auto outerContainer = ContainerBuilder()
                              .padding(10)
                              .margin(5)
                              .backgroundColor(Color::Red)
                              .border(true, Color::White)
                              .child(std::move(innerContainer))
                              .build();

    outerContainer->measure(135, 240);
    outerContainer->layout(0, 0);

    TEST_ASSERT_EQUAL(10, outerContainer->getPadding());
    TEST_ASSERT_EQUAL(5, outerContainer->getMargin());
}

void test_canvas_with_callback()
{
    bool callbackExecuted = false;

    auto canvas = CanvasBuilder()
                      .size(100, 100)
                      .backgroundColor(Color::Black)
                      .drawCallback([&callbackExecuted](TFT_eSPI& display, int x, int y, int width, int height)
                                    {
                                        callbackExecuted = true;
                                        // Would normally draw shapes here using display, x, y, width, height
                                    })
                      .build();

    canvas->measure(135, 240);
    canvas->layout(0, 0);

    // Note: callback won't execute until render() is called
    TEST_ASSERT_TRUE(canvas->isVisible());
}

void test_complex_layout()
{
    auto header = TextBuilder("Header")
                      .fontSize(3)
                      .color(Color::White)
                      .align(Align::CENTER)
                      .build();

    auto headerContainer = ContainerBuilder()
                               .padding(10)
                               .backgroundColor(Color::Blue)
                               .child(std::move(header))
                               .build();

    auto body1 = TextBuilder("Body line 1").fontSize(2).build();
    auto body2 = TextBuilder("Body line 2").fontSize(2).build();

    auto column = ColumnBuilder()
                      .spacing(10)
                      .mainAxisAlignment(MainAxisAlignment::START)
                      .addChild(std::move(headerContainer))
                      .addChild(std::move(body1))
                      .addChild(std::move(body2))
                      .build();

    column->measure(135, 240);
    column->layout(0, 0);

    TEST_ASSERT_TRUE(column->isVisible());
    TEST_ASSERT_GREATER_THAN(0, column->getHeight());
}
