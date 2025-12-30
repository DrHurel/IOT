#pragma once

#include <unity.h>
#include "testing/libs/common/ui/MockDisplay.h"
#include "libs/common/ui/UI.h"
#include <memory>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <string>

using namespace common::ui;
using namespace testing::mocks;

// Global test state
extern std::unique_ptr<MockDisplay> display;
extern const std::string SNAPSHOT_DIR;
extern bool UPDATE_SNAPSHOTS;

// Common test setup and teardown - Unity will call these automatically
void setUp();
void tearDown();

// Helper function to compare or update snapshots
inline bool assertSnapshot(const std::string &testName)
{
    std::string snapshotPath = SNAPSHOT_DIR + testName + ".snapshot";
    std::string actual = display->generateSnapshot();

    if (UPDATE_SNAPSHOTS)
    {
        display->saveSnapshot(snapshotPath);
        return true;
    }

    if (!std::filesystem::exists(snapshotPath))
    {
        // First run - create snapshot
        display->saveSnapshot(snapshotPath);
        TEST_MESSAGE((std::string("Created snapshot: ") + testName + ".snapshot").c_str());
        return true;
    }

    std::string expected = display->loadSnapshot(snapshotPath);
    if (actual != expected)
    {
        // Save the actual output for debugging
        display->saveSnapshot(snapshotPath + ".actual");
        TEST_MESSAGE((std::string("Snapshot mismatch! Actual saved to: ") + testName + ".snapshot.actual").c_str());
        return false;
    }

    return true;
}

// Create a RenderContext using the MockDisplay
inline RenderContext createMockRenderContext()
{
    // We need to cast MockDisplay to TFT_eSPI reference
    // This is a workaround - in production we'd need a better abstraction
    return RenderContext(0, 0, display->width(), display->height(),
                         *reinterpret_cast<TFT_eSPI *>(display.get()));
}
