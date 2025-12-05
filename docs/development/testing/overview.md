# Testing Overview

> **Note:** This page is under construction. Detailed testing documentation coming soon!

PlantNanny includes a comprehensive testing framework to ensure code quality and reliability.

## Testing Strategy

### Test Pyramid

```
        /\
       /  \
      / E2E\         End-to-End Tests (Hardware)
     /______\
    /        \
   / Integr. \      Integration Tests
  /___________\
 /             \
/  Unit Tests   \   Unit Tests (Native & Hardware)
/_________________\
```

## Test Types

### 1. Unit Tests

Fast, isolated tests for individual components:

- **Run on**: Native platform (your development machine)
- **Framework**: Unity test framework
- **Coverage**: Utilities, patterns, UI components
- **Location**: `test/test_*/`

### 2. UI Snapshot Tests

Visual regression testing for UI components:

- **Run on**: Native platform with MockDisplay
- **Framework**: Custom snapshot system
- **Coverage**: All UI components
- **Location**: `test/test_ui_snapshot/`

See [UI Snapshot Testing](/development/ui/snapshot-testing) for details.

### 3. Integration Tests

Test component interactions:

- **Run on**: Hardware and native
- **Framework**: Unity
- **Coverage**: Network, Bluetooth, sensors
- **Location**: `test/test_network/`, `test/test_bluetooth/`

### 4. Hardware Tests

Tests requiring actual hardware:

- **Run on**: ESP32 hardware only
- **Framework**: Unity
- **Coverage**: Real WiFi, OTA updates
- **Location**: `test/test_real_wifi/`, `test/test_ota/`

## Running Tests

### Run All Tests

```bash
# Run all tests on native platform
pio test -e native

# Or use helper script
./Devtools/test.sh
```

### Run Specific Test Suite

```bash
# Run specific test
pio test -e native -f test_ui_snapshot

# Run with verbose output
pio test -e native -f test_network -v
```

### Run on Hardware

```bash
# Upload and run tests on device
pio test -e lilygo-t-display-test --upload-port /dev/ttyUSB0
```

## Test Structure

### Basic Test

```cpp
#include <unity.h>

void test_example() {
    // Arrange
    int expected = 42;
    
    // Act
    int result = calculateAnswer();
    
    // Assert
    TEST_ASSERT_EQUAL(expected, result);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(test_example);
    UNITY_END();
}

void loop() {
    // Empty for tests
}
```

### Test File Template

```cpp
#include <unity.h>
#include "YourClass.h"

// Test setup (optional)
void setUp() {
    // Called before each test
}

// Test teardown (optional)
void tearDown() {
    // Called after each test
}

// Test cases
void test_feature_works() {
    YourClass obj;
    TEST_ASSERT_TRUE(obj.feature());
}

void test_feature_fails_with_invalid_input() {
    YourClass obj;
    TEST_ASSERT_FALSE(obj.feature(-1));
}

// Main test runner
void setup() {
    delay(2000);  // Wait for serial
    
    UNITY_BEGIN();
    RUN_TEST(test_feature_works);
    RUN_TEST(test_feature_fails_with_invalid_input);
    UNITY_END();
}

void loop() {}
```

## Unity Assertions

### Equality

```cpp
TEST_ASSERT_EQUAL(expected, actual);
TEST_ASSERT_EQUAL_INT(42, result);
TEST_ASSERT_EQUAL_FLOAT(3.14, pi, 0.01);
TEST_ASSERT_EQUAL_STRING("hello", str);
```

### Boolean

```cpp
TEST_ASSERT_TRUE(condition);
TEST_ASSERT_FALSE(condition);
TEST_ASSERT(condition);  // Same as TRUE
```

### Null

```cpp
TEST_ASSERT_NULL(pointer);
TEST_ASSERT_NOT_NULL(pointer);
```

### Comparison

```cpp
TEST_ASSERT_GREATER_THAN(5, result);
TEST_ASSERT_LESS_THAN(10, result);
TEST_ASSERT_GREATER_OR_EQUAL(5, result);
```

### Arrays

```cpp
TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, count);
TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, count);
```

## Test Organization

```
test/
├── test_common_init.cpp       # Common initialization
├── test_basic/                # Basic functionality
├── test_ui_snapshot/          # UI snapshot tests
├── test_network/              # Network tests (mocked)
├── test_bluetooth/            # Bluetooth tests
├── test_ota/                  # OTA update tests
├── test_real_wifi/            # Real WiFi tests (hardware)
└── snapshots/                 # Snapshot storage
    └── ui/
```

## Mocking

### Mock Display

For UI testing without hardware:

```cpp
#include "test/mocks/MockDisplay.h"

void test_ui_component() {
    MockDisplay display;
    
    auto text = TextBuilder("Test").build();
    text->draw(&display);
    
    TEST_ASSERT_TRUE(display.wasDrawn());
}
```

### Mock Network (Planned)

For network testing without connectivity:

```cpp
MockWiFi wifi;
wifi.setConnected(true);
TEST_ASSERT_TRUE(wifi.isConnected());
```

## Continuous Integration

Tests are run automatically on:
- Pull requests
- Commits to main branch
- Nightly builds

### CI Configuration (Planned)

```yaml
# .github/workflows/test.yml
name: Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Run tests
        run: pio test -e native
```

## Code Coverage

Generate coverage reports:

```bash
# Run with coverage
pio test -e native --coverage

# Generate HTML report
genhtml coverage.info -o coverage_html
```

## Best Practices

### 1. Test Naming

Use descriptive names:
```cpp
// Good
void test_temperature_reading_within_valid_range()

// Avoid
void test1()
```

### 2. One Assertion Per Test

```cpp
// Good
void test_temperature_is_positive() {
    TEST_ASSERT_GREATER_THAN(0, temp);
}

void test_temperature_within_range() {
    TEST_ASSERT_LESS_THAN(50, temp);
}

// Avoid (multiple concepts)
void test_temperature() {
    TEST_ASSERT_GREATER_THAN(0, temp);
    TEST_ASSERT_LESS_THAN(50, temp);
    TEST_ASSERT_NOT_NULL(sensor);
}
```

### 3. Arrange-Act-Assert

```cpp
void test_water_pump() {
    // Arrange
    WaterPump pump(PIN);
    
    // Act
    pump.activate(200);
    
    // Assert
    TEST_ASSERT_TRUE(pump.isActive());
}
```

### 4. Clean Up Resources

```cpp
void tearDown() {
    // Clean up after each test
    delete globalObject;
    globalObject = nullptr;
}
```

## Debugging Tests

### Serial Output

```cpp
void test_debug() {
    Serial.println("Test starting...");
    int result = calculate();
    Serial.printf("Result: %d\n", result);
    TEST_ASSERT_EQUAL(42, result);
}
```

### Conditional Tests

```cpp
#ifdef DEBUG
void test_debug_feature() {
    // Only run in debug builds
}
#endif
```

## Test Configuration

Configure in `platformio.ini`:

```ini
[env:native]
platform = native
test_framework = unity
test_build_src = yes
test_ignore = 
    test_embedded_only/*
    test_real_wifi/*
```

## Next Steps

- [Unit Tests](/development/testing/unit-tests) - Write unit tests
- [Network Tests](/development/testing/network-tests) - Test networking
- [UI Snapshot Testing](/development/ui/snapshot-testing) - Test UI components

---

*Full documentation coming soon. Check back for updates!*
