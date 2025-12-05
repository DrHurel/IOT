#include "test_ui_helpers.h"
#include "libs/common/ui/Builders.h"
#include "libs/common/ui/components/Text.h"
#include "libs/common/ui/components/Container.h"
#include "libs/common/ui/components/Column.h"
#include "libs/common/ui/components/Horizontal.h"
#include "libs/common/ui/components/Canvas.h"

using namespace common::ui::components;

// ============================================================================
// Container and Layout Visual Snapshot Tests - These generate PNG images
// ============================================================================

void test_container_visual_padding_margin()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto text = TextBuilder("Padded").fontSize(2).color(Color::White).build();

    auto container = ContainerBuilder()
                         .padding(10)
                         .margin(15)
                         .backgroundColor(Color::Blue)
                         .border(true, Color::White)
                         .child(std::move(text))
                         .build();

    container->measure(135, 240);
    container->layout(0, 0);

    auto context = createMockRenderContext();
    container->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("container_padding_margin"));
}

void test_container_visual_border()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto text = TextBuilder("Border").fontSize(2).color(Color::Yellow).build();

    auto container = ContainerBuilder()
                         .padding(8)
                         .backgroundColor(Color::Red)
                         .border(true, Color::White)
                         .child(std::move(text))
                         .build();

    container->measure(135, 240);
    container->layout(10, 10);

    auto context = createMockRenderContext();
    container->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("container_border"));
}

void test_column_visual_spacing()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto column = ColumnBuilder()
                      .spacing(10)
                      .backgroundColor(Color::DarkGray)
                      .addChild(TextBuilder("Item 1").color(Color::White).build())
                      .addChild(TextBuilder("Item 2").color(Color::Cyan).build())
                      .addChild(TextBuilder("Item 3").color(Color::Yellow).build())
                      .build();

    column->measure(135, 240);
    column->layout(5, 5);

    auto context = createMockRenderContext();
    column->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("column_spacing"));
}

void test_column_visual_alignment()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto column = ColumnBuilder()
                      .mainAxisAlignment(MainAxisAlignment::CENTER)
                      .crossAxisAlignment(CrossAxisAlignment::CENTER)
                      .spacing(5)
                      .backgroundColor(Color::Blue)
                      .addChild(TextBuilder("Centered").color(Color::White).build())
                      .addChild(TextBuilder("Content").color(Color::Yellow).build())
                      .build();

    column->measure(135, 240);
    column->layout(0, 0);

    auto context = createMockRenderContext();
    column->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("column_alignment"));
}

void test_row_visual_horizontal_layout()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto row = RowBuilder()
                   .spacing(5)
                   .backgroundColor(Color::DarkGray)
                   .addChild(TextBuilder("A").color(Color::Red).build())
                   .addChild(TextBuilder("B").color(Color::Green).build())
                   .addChild(TextBuilder("C").color(Color::Blue).build())
                   .build();

    row->measure(135, 240);
    row->layout(5, 5);

    auto context = createMockRenderContext();
    row->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("row_horizontal"));
}

void test_row_visual_alignment()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto row = RowBuilder()
                   .mainAxisAlignment(MainAxisAlignment::CENTER)
                   .crossAxisAlignment(CrossAxisAlignment::CENTER)
                   .spacing(10)
                   .backgroundColor(Color::Blue)
                   .addChild(TextBuilder("1").fontSize(2).color(Color::White).build())
                   .addChild(TextBuilder("2").fontSize(2).color(Color::Yellow).build())
                   .addChild(TextBuilder("3").fontSize(2).color(Color::Cyan).build())
                   .build();

    row->measure(135, 240);
    row->layout(0, 0);

    auto context = createMockRenderContext();
    row->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("row_alignment"));
}

void test_canvas_visual_drawing()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto canvas = CanvasBuilder()
                      .size(120, 100)
                      .backgroundColor(Color::DarkGray)
                      .drawCallback([](TFT_eSPI& display, int x, int y, int width, int height)
                                    {
                                        // Draw a simple pattern
                                        display.drawLine(x, y, x + width, y + height, static_cast<uint16_t>(Color::Red));
                                        display.drawLine(x + width, y, x, y + height, static_cast<uint16_t>(Color::Blue));
                                        display.drawCircle(x + width/2, y + height/2, 20, static_cast<uint16_t>(Color::Green));
                                    })
                      .build();

    canvas->measure(135, 240);
    canvas->layout(10, 10);

    auto context = createMockRenderContext();
    canvas->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("canvas_drawing"));
}

void test_nested_layout_visual()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    // Create a complex nested layout
    auto header = TextBuilder("Header").fontSize(2).color(Color::White).align(Align::CENTER).build();
    auto headerContainer = ContainerBuilder()
                               .padding(8)
                               .backgroundColor(Color::Blue)
                               .child(std::move(header))
                               .build();

    auto row = RowBuilder()
                   .spacing(5)
                   .addChild(TextBuilder("L").color(Color::Red).build())
                   .addChild(TextBuilder("M").color(Color::Green).build())
                   .addChild(TextBuilder("R").color(Color::Blue).build())
                   .build();

    auto footer = TextBuilder("Footer").fontSize(1).color(Color::Gray).build();

    auto mainColumn = ColumnBuilder()
                          .spacing(10)
                          .backgroundColor(Color::Black)
                          .addChild(std::move(headerContainer))
                          .addChild(std::move(row))
                          .addChild(std::move(footer))
                          .build();

    mainColumn->measure(135, 240);
    mainColumn->layout(0, 0);

    auto context = createMockRenderContext();
    mainColumn->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("nested_layout"));
}

void test_all_components_showcase()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    // Title
    auto title = TextBuilder("Components").fontSize(3).color(Color::White).align(Align::CENTER).build();

    // Container example
    auto containerText = TextBuilder("Box").fontSize(1).color(Color::White).build();
    auto containerBox = ContainerBuilder()
                            .padding(5)
                            .backgroundColor(Color::Red)
                            .border(true, Color::White)
                            .child(std::move(containerText))
                            .build();

    // Row example
    auto colorRow = RowBuilder()
                        .spacing(3)
                        .addChild(TextBuilder("R").color(Color::Red).build())
                        .addChild(TextBuilder("G").color(Color::Green).build())
                        .addChild(TextBuilder("B").color(Color::Blue).build())
                        .build();

    // Canvas example (small)
    auto smallCanvas = CanvasBuilder()
                           .size(60, 40)
                           .backgroundColor(Color::DarkGray)
                           .drawCallback([](TFT_eSPI& display, int x, int y, int w, int h)
                                         {
                                             display.fillCircle(x + w/2, y + h/2, 15, static_cast<uint16_t>(Color::Cyan));
                                         })
                           .build();

    // Main layout
    auto showcase = ColumnBuilder()
                        .spacing(8)
                        .mainAxisAlignment(MainAxisAlignment::START)
                        .addChild(std::move(title))
                        .addChild(std::move(containerBox))
                        .addChild(std::move(colorRow))
                        .addChild(std::move(smallCanvas))
                        .build();

    showcase->measure(135, 240);
    showcase->layout(0, 5);

    auto context = createMockRenderContext();
    showcase->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("all_components_showcase"));
}
