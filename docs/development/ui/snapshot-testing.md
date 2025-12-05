# UI Snapshot Testing Implementation Summary

## What Was Implemented

A complete snapshot testing framework for the common/ui library that allows testing UI components on native platforms without hardware dependencies.

## Files Created

### 1. Mock Display System
- **`test/mocks/MockDisplay.h`** - Mock display class that captures rendering operations
- **`test/mocks/MockDisplay.cpp`** - Implementation with snapshot generation logic
- **`test/mocks/TFT_eSPI.h`** - Mock TFT_eSPI header for native compilation

### 2. Test Infrastructure
- **`test/test_ui_snapshot/test_ui_snapshot.cpp`** - Test cases with snapshot assertions
- **`test/test_ui_snapshot/README.md`** - Comprehensive testing guide
- **`Devtools/test_ui_snapshots.sh`** - Helper script for running tests

### 3. Snapshot Storage
- **`test/snapshots/ui/`** - Directory for storing snapshot files
- **`test/snapshots/ui/README.md`** - Documentation about snapshots
- **`test/snapshots/ui/.gitignore`** - Ignore temporary .actual files

### 4. Configuration
- **`platformio.ini`** - Updated native environment to exclude problematic UI files

## How It Works

### Architecture

```
Test Code → UI Component → MockDisplay → Snapshot File
                                ↓
                         Record Operations
                         (fillRect, drawText, etc.)
                                ↓
                         Generate Text Output
                                ↓
                         Compare with Saved Snapshot
```

### Test Flow

1. **Setup**: Create a `MockDisplay` instance
2. **Build**: Construct UI components using builders
3. **Layout**: Call `measure()` and `layout()` on components
4. **Render**: Components draw to MockDisplay (or test directly)
5. **Assert**: Compare captured operations against saved snapshot
6. **Teardown**: Clean up resources

## Example Test

```cpp
void test_text_component_simple()
{
    // Build component
    auto text = TextBuilder("Hello World")
        .fontSize(2)
        .color(Color::White)
        .build();

    // Layout
    text->measure(135, 240);
    text->layout(10, 20);

    // Assert properties
    TEST_ASSERT_EQUAL_STRING("Hello World", text->getText().c_str());
    TEST_ASSERT_EQUAL(10, text->getX());
}
```

## Running Tests

### Basic Usage
```bash
# Run all UI snapshot tests
pio test -e native -f test_ui_snapshot

# Or use the helper script
./Devtools/test_ui_snapshots.sh
```

### Update Snapshots
```bash
# When UI changes are intentional
UPDATE_SNAPSHOTS=1 pio test -e native -f test_ui_snapshot

# Or
./Devtools/test_ui_snapshots.sh --update
```

### Verbose Output
```bash
./Devtools/test_ui_snapshots.sh --verbose
```

## Snapshot Format

Snapshots are human-readable text files showing rendering operations:

```
Display(135x240)
Operations: 5
---
0000: CLEAR(color=0x0000)
0001: SET_TEXT_COLOR(fg=0xffff)
0002: SET_TEXT_SIZE(size=2)
0003: DRAW_TEXT("Hello World") at(x=10, y=20) color=0xffff size=2
0004: FILL_RECT(x=0, y=0, w=135, h=50) color=0x001f
```

## MockDisplay API

The MockDisplay implements these TFT_eSPI-compatible methods:

- `fillRect(x, y, w, h, color)`
- `drawString(text, x, y)`
- `drawPixel(x, y, color)`
- `fillScreen(color)`
- `setTextColor(color)` / `setTextColor(fg, bg)`
- `setTextSize(size)`
- `setCursor(x, y)`
- `print(text)`
- `drawLine(x0, y0, x1, y1, color)`
- `drawCircle(x, y, radius, color)`
- `fillCircle(x, y, radius, color)`

Plus snapshot methods:

- `generateSnapshot()` - Create text representation
- `saveSnapshot(filename)` - Save to file
- `loadSnapshot(filename)` - Load from file
- `compareSnapshot(expected)` - Compare strings
- `clear()` - Reset state

## Test Cases Included

1. **Mock Display Tests**
   - Basic operations (fillScreen, drawString, fillRect)
   - Multiple shapes (lines, circles)

2. **Component Tests**
   - Simple text components
   - Text with different colors
   - Containers with padding/margins
   - Containers with borders
   - Column layouts
   - Nested containers
   - Canvas with callbacks
   - Complex multi-level layouts

## Benefits

✅ **Fast** - No hardware required, runs in milliseconds
✅ **Deterministic** - Same input always produces same output  
✅ **Visible** - Changes are visible in git diffs
✅ **Automated** - Runs in CI/CD pipelines
✅ **Comprehensive** - Tests entire rendering pipeline
✅ **Debuggable** - Snapshot text is human-readable

## Limitations & Future Improvements

### Current Limitations

1. **No Pixel Testing** - Tests operations, not final pixels
2. **No Visual Preview** - Text output only
3. **Manual Hardware Verification** - Still need to test on device
4. **Render Context Coupling** - RenderContext requires TFT_eSPI reference

### Future Enhancements

1. **Framebuffer Testing**
   ```cpp
   // Generate actual pixel data
   Framebuffer fb(135, 240);
   component->render(fb);
   fb.saveAsPNG("snapshot.png");
   ```

2. **Visual Diff Tool**
   ```bash
   # Compare snapshots visually
   ./compare_snapshots.sh test1.snapshot test2.snapshot
   ```

3. **Better Abstraction**
   ```cpp
   // Abstract display interface
   class IDisplay {
       virtual void fillRect(...) = 0;
       // ...
   };
   ```

4. **Performance Metrics**
   ```cpp
   // Track rendering performance
   TEST_ASSERT_LESS_THAN(100, display->getOperationCount());
   ```

## Integration with CI/CD

Add to `.github/workflows/test.yml`:

```yaml
test-ui-snapshots:
  runs-on: ubuntu-latest
  steps:
    - uses: actions/checkout@v3
    
    - name: Setup PlatformIO
      uses: actions/setup-python@v4
      with:
        python-version: '3.x'
    
    - run: pip install platformio
    
    - name: Run UI Snapshot Tests
      run: pio test -e native -f test_ui_snapshot
    
    - name: Check for unexpected snapshot changes
      run: |
        if git diff --exit-code test/snapshots/ui/*.snapshot; then
          echo "✓ Snapshots are up to date"
        else
          echo "✗ Snapshots have changed unexpectedly!"
          git diff test/snapshots/ui/
          exit 1
        fi
```

## Troubleshooting

### Compilation Errors

**Error**: `TFT_eSPI.h not found`
- **Solution**: Make sure `test/mocks` is in include path (check platformio.ini)

**Error**: `get_display() undefined reference`
- **Solution**: The `core.cpp` is excluded in native builds - tests should use MockDisplay directly

### Test Failures

**Snapshot mismatch**
1. Check the `.actual` file: `cat test/snapshots/ui/test_name.snapshot.actual`
2. Compare with expected: `diff test/snapshots/ui/test_name.snapshot{,.actual}`
3. If intentional: `UPDATE_SNAPSHOTS=1 pio test -e native -f test_ui_snapshot`

**Missing snapshot files**
- First run creates snapshots automatically
- Run tests again to perform actual comparison

## Next Steps

1. **Add More Tests** - Cover more component combinations
2. **Test Interactions** - Button clicks, scrolling, animations
3. **Performance Tests** - Measure rendering efficiency
4. **Visual Regression** - Add pixel-based testing
5. **Documentation** - Add examples for common patterns

## Resources

- PlatformIO Testing: https://docs.platformio.org/en/latest/advanced/unit-testing/
- Unity Test Framework: https://github.com/ThrowTheSwitch/Unity
- Snapshot Testing Concepts: https://jestjs.io/docs/snapshot-testing

## Questions?

See the detailed guides:
- `test/test_ui_snapshot/README.md` - Testing guide
- `test/snapshots/ui/README.md` - Snapshot documentation
- Run `./Devtools/test_ui_snapshots.sh --help` - Script usage
