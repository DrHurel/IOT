# UI Snapshot Testing - Quick Reference

## 🚀 Quick Start

```bash
# Run tests
pio test -e native -f test_ui_snapshot

# Update snapshots
UPDATE_SNAPSHOTS=1 pio test -e native -f test_ui_snapshot

# Use helper script
./Devtools/test_ui_snapshots.sh --update
```

## 📝 Write a Test

```cpp
void test_my_component()
{
    // 1. Build component
    auto text = TextBuilder("Hello")
        .fontSize(2)
        .color(Color::White)
        .build();

    // 2. Layout
    text->measure(135, 240);
    text->layout(10, 20);

    // 3. Test properties
    TEST_ASSERT_EQUAL_STRING("Hello", text->getText().c_str());
    TEST_ASSERT_EQUAL(10, text->getX());
}

// Register in main()
RUN_TEST(test_my_component);
```

## 🧪 Test with Snapshots

```cpp
void test_with_snapshot()
{
    display->fillScreen(0x0000);
    display->drawString("Test", 10, 10);
    
    TEST_ASSERT_TRUE(assertSnapshot("my_snapshot"));
}
```

## 📂 File Structure

```
test/
├── mocks/
│   ├── MockDisplay.h          # Display mock
│   ├── MockDisplay.cpp
│   └── TFT_eSPI.h            # TFT mock
├── test_ui_snapshot/
│   ├── test_ui_snapshot.cpp  # Tests
│   └── README.md
└── snapshots/ui/
    └── *.snapshot            # Saved snapshots
Devtools/
└── test_ui_snapshots.sh      # Helper script
```

## 🔍 Debug Failed Tests

```bash
# View actual output
cat test/snapshots/ui/my_test.snapshot.actual

# Compare with expected
diff test/snapshots/ui/my_test.snapshot{,.actual}

# Update if correct
UPDATE_SNAPSHOTS=1 pio test -e native -f test_ui_snapshot
```

## 🎨 Available Builders

```cpp
// Text
TextBuilder("Hello")
    .fontSize(2)
    .color(Color::White)
    .align(Align::CENTER)
    .underline(true)
    .build();

// Container
ContainerBuilder()
    .padding(10)
    .margin(5)
    .backgroundColor(Color::Blue)
    .border(true, Color::White)
    .child(std::move(component))
    .build();

// Column
ColumnBuilder()
    .mainAxisAlignment(MainAxisAlignment::CENTER)
    .crossAxisAlignment(CrossAxisAlignment::CENTER)
    .spacing(5)
    .addChild(std::move(child1))
    .addChild(std::move(child2))
    .build();

// Canvas
CanvasBuilder()
    .size(100, 100)
    .backgroundColor(Color::Black)
    .drawCallback([](Canvas& c, RenderContext& ctx) {
        // Custom drawing
    })
    .build();
```

## 🎯 Common Assertions

```cpp
// Component properties
TEST_ASSERT_EQUAL(expected, actual);
TEST_ASSERT_EQUAL_STRING("text", component->getText().c_str());
TEST_ASSERT_TRUE(component->isVisible());
TEST_ASSERT_GREATER_THAN(0, component->getHeight());

// Snapshots
TEST_ASSERT_TRUE(assertSnapshot("test_name"));
TEST_ASSERT_EQUAL(5, display->getOperationCount());
```

## 🌈 Available Colors

```cpp
Color::Black, Color::White, Color::Red, Color::Green, 
Color::Blue, Color::Yellow, Color::Cyan, Color::Magenta,
Color::Gray, Color::DarkGray, Color::LightGray
```

## 💡 Tips

- ✅ One component per test
- ✅ Descriptive test names
- ✅ Test different states
- ✅ Commit snapshots to git
- ✅ Review snapshot diffs
- ❌ Don't test implementation details
- ❌ Don't couple tests together

## 📚 More Info

- Full guide: `test/test_ui_snapshot/README.md`
- Implementation: `docs/ui-snapshot-testing.md`
- Help: `./Devtools/test_ui_snapshots.sh --help`
