#include "testing/ui/helpers.h"

using namespace common::ui;
using namespace testing::mocks;

// Global test state definitions
std::unique_ptr<MockDisplay> display = nullptr;
const std::string SNAPSHOT_DIR = std::string(PROJECT_DIR) + "/test/snapshots/ui/";
bool UPDATE_SNAPSHOTS = false;

// Override the display getter for this test
testing::mocks::MockDisplay *get_test_display_ptr()
{
    return display.get();
}

// Unity setup and teardown - called before/after each test
void setUp()
{
    display = std::make_unique<MockDisplay>(135, 240);
}

void tearDown()
{
    display.reset();
}

// Test function declarations from separate .cpp files
// Mock Display tests (test_ui_mock_display.cpp)
void test_mock_display_basic_operations();
void test_mock_display_multiple_shapes();

// Component Property tests (test_ui_components.cpp)
void test_text_component_simple();
void test_text_component_different_colors();
void test_container_with_padding();
void test_container_with_border();
void test_column_layout_multiple_children();
void test_nested_containers();
void test_canvas_with_callback();
void test_complex_layout();

// Text Visual tests (test_ui_text_visual.cpp)
void test_text_visual_left_align();
void test_text_visual_center_align();
void test_text_visual_right_align();
void test_text_visual_colors();
void test_text_visual_sizes();

// Layout Visual tests (test_ui_layout_visual.cpp)
void test_container_visual_padding_margin();
void test_container_visual_border();
void test_column_visual_spacing();
void test_column_visual_alignment();
void test_row_visual_horizontal_layout();
void test_row_visual_alignment();
void test_canvas_visual_drawing();
void test_nested_layout_visual();
void test_all_components_showcase();

// ============================================================================
// Main Test Runner
// ============================================================================

int main(int argc, char **argv)
{
    // Check if we should update snapshots
    const auto updateEnv = std::getenv("UPDATE_SNAPSHOTS");
    if (updateEnv != nullptr && std::string(updateEnv) == "1")
    {
        UPDATE_SNAPSHOTS = true;
        printf("UPDATE_SNAPSHOTS mode enabled\n");
    }

    // Create snapshot directory if it doesn't exist
    std::filesystem::create_directories(SNAPSHOT_DIR);

    UNITY_BEGIN();

    // Mock Display Tests
    RUN_TEST(test_mock_display_basic_operations);
    RUN_TEST(test_mock_display_multiple_shapes);

    // Component Property Tests (no rendering)
    RUN_TEST(test_text_component_simple);
    RUN_TEST(test_text_component_different_colors);
    RUN_TEST(test_container_with_padding);
    RUN_TEST(test_container_with_border);
    RUN_TEST(test_column_layout_multiple_children);
    RUN_TEST(test_nested_containers);
    RUN_TEST(test_canvas_with_callback);
    RUN_TEST(test_complex_layout);

    // Visual Snapshot Tests (PNG generation)
    RUN_TEST(test_text_visual_left_align);
    RUN_TEST(test_text_visual_center_align);
    RUN_TEST(test_text_visual_right_align);
    RUN_TEST(test_text_visual_colors);
    RUN_TEST(test_text_visual_sizes);
    RUN_TEST(test_container_visual_padding_margin);
    RUN_TEST(test_container_visual_border);
    RUN_TEST(test_column_visual_spacing);
    RUN_TEST(test_column_visual_alignment);
    RUN_TEST(test_row_visual_horizontal_layout);
    RUN_TEST(test_row_visual_alignment);
    RUN_TEST(test_canvas_visual_drawing);
    RUN_TEST(test_nested_layout_visual);
    RUN_TEST(test_all_components_showcase);

    return UNITY_END();
}
