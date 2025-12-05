#include <unity.h>
#include "test_ui_helpers.h"
#include "libs/common/ui/Builders.h"
#include "libs/common/ui/components/Text.h"

using namespace common::ui::components;

// ============================================================================
// Text Visual Snapshot Tests - These generate PNG images
// ============================================================================

void test_text_visual_left_align()
{
    TEST_MESSAGE("Starting test_text_visual_left_align");
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto text = TextBuilder("Left Aligned")
                    .fontSize(2)
                    .color(Color::White)
                    .align(Align::LEFT)
                    .build();

    text->measure(135, 240);
    text->layout(0, 10);

    auto context = createMockRenderContext();
    TEST_MESSAGE("About to render");
    text->render(context);
    TEST_MESSAGE("Render complete");

    TEST_ASSERT_TRUE(assertSnapshot("text_left_align"));
}

void test_text_visual_center_align()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto text = TextBuilder("Centered")
                    .fontSize(2)
                    .color(Color::Cyan)
                    .align(Align::CENTER)
                    .build();

    text->measure(135, 240);
    text->layout(0, 50);

    auto context = createMockRenderContext();
    text->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("text_center_align"));
}

void test_text_visual_right_align()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto text = TextBuilder("Right")
                    .fontSize(2)
                    .color(Color::Yellow)
                    .align(Align::RIGHT)
                    .build();

    text->measure(135, 240);
    text->layout(0, 90);

    auto context = createMockRenderContext();
    text->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("text_right_align"));
}

void test_text_visual_colors()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto red = TextBuilder("Red").fontSize(2).color(Color::Red).build();
    auto green = TextBuilder("Green").fontSize(2).color(Color::Green).build();
    auto blue = TextBuilder("Blue").fontSize(2).color(Color::Blue).build();
    auto white = TextBuilder("White").fontSize(2).color(Color::White).build();

    red->measure(135, 240); red->layout(10, 10);
    green->measure(135, 240); green->layout(10, 40);
    blue->measure(135, 240); blue->layout(10, 70);
    white->measure(135, 240); white->layout(10, 100);

    auto context = createMockRenderContext();
    red->render(context);
    green->render(context);
    blue->render(context);
    white->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("text_colors"));
}

void test_text_visual_sizes()
{
    display->fillScreen(static_cast<uint16_t>(Color::Black));

    auto size1 = TextBuilder("Size 1").fontSize(1).color(Color::White).build();
    auto size2 = TextBuilder("Size 2").fontSize(2).color(Color::White).build();
    auto size3 = TextBuilder("Size 3").fontSize(3).color(Color::White).build();

    size1->measure(135, 240); size1->layout(5, 10);
    size2->measure(135, 240); size2->layout(5, 25);
    size3->measure(135, 240); size3->layout(5, 50);

    auto context = createMockRenderContext();
    size1->render(context);
    size2->render(context);
    size3->render(context);

    TEST_ASSERT_TRUE(assertSnapshot("text_sizes"));
}
