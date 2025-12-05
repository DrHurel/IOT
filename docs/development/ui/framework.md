# UI Framework

PlantNanny includes a custom UI framework for building component-based interfaces on embedded displays.

## Overview

The UI framework provides a declarative, component-based approach to building user interfaces for the TFT display on the ESP32 controller.

### Key Features

- **Component-Based**: Reusable UI components (Text, Container, etc.)
- **Fluent Builder API**: Chainable builder pattern for easy component creation
- **Layout System**: Automatic layout calculation with flexbox-like behavior
- **Type-Safe**: Modern C++23 with compile-time safety
- **Testable**: Full snapshot testing support on native platforms
- **Lightweight**: Optimized for embedded systems

## Architecture

```
UI Manager
├── Component Tree
│   ├── Container
│   │   ├── Text
│   │   └── Text
│   └── Container
│       └── Button
├── Layout Engine
│   ├── Measure
│   └── Layout
└── Renderer
    └── TFT_eSPI Display
```

## Core Concepts

### Components

Components are the building blocks of the UI:

```cpp
class Component {
public:
    virtual void measure(int availableWidth, int availableHeight) = 0;
    virtual void layout(int x, int y) = 0;
    virtual void draw(TFT_eSPI* display) = 0;
    
    // Getters for dimensions
    int getWidth() const;
    int getHeight() const;
    int getX() const;
    int getY() const;
};
```

### Layout Process

1. **Measure**: Calculate required dimensions
2. **Layout**: Position components at coordinates
3. **Draw**: Render to display

```cpp
// Build component
auto component = TextBuilder("Hello").build();

// Measure (determine size)
component->measure(screenWidth, screenHeight);

// Layout (set position)
component->layout(x, y);

// Draw (render to display)
component->draw(display);
```

## Available Components

### Text

Display text with styling:

```cpp
auto text = TextBuilder("Hello World")
    .fontSize(2)
    .color(Color::White)
    .backgroundColor(Color::Black)
    .align(Align::CENTER)
    .underline(true)
    .bold(true)
    .build();
```

Properties:
- `fontSize(int)` - Font size (1-8)
- `color(Color)` - Text color
- `backgroundColor(Color)` - Background color
- `align(Align)` - Text alignment (LEFT, CENTER, RIGHT)
- `underline(bool)` - Underline text
- `bold(bool)` - Bold text (simulated)

### Container

Group and position child components:

```cpp
auto container = ContainerBuilder()
    .padding(10)
    .margin(5)
    .backgroundColor(Color::Blue)
    .border(true, Color::White)
    .borderWidth(2)
    .child(std::move(textComponent))
    .child(std::move(anotherComponent))
    .build();
```

Properties:
- `padding(int)` - Internal spacing
- `margin(int)` - External spacing
- `backgroundColor(Color)` - Background fill
- `border(bool, Color)` - Draw border
- `borderWidth(int)` - Border thickness
- `child(Component)` - Add child component

### Button (Planned)

Interactive button component:

```cpp
auto button = ButtonBuilder("Click Me")
    .onPress([]() { /* handler */ })
    .backgroundColor(Color::Green)
    .build();
```

### Image (Planned)

Display images and icons:

```cpp
auto image = ImageBuilder()
    .source("/images/icon.png")
    .width(64)
    .height(64)
    .build();
```

## Builder Pattern

All components use the builder pattern for construction:

### Benefits

- **Readability**: Clear, self-documenting code
- **Flexibility**: Optional parameters with defaults
- **Type Safety**: Compile-time validation
- **Chainable**: Fluent API for clean syntax

### Example

```cpp
// Build a styled text component
auto title = TextBuilder("Plant Status")
    .fontSize(3)
    .color(Color::Green)
    .align(Align::CENTER)
    .build();

// Build a container with children
auto panel = ContainerBuilder()
    .padding(10)
    .backgroundColor(Color::DarkGray)
    .child(std::move(title))
    .child(TextBuilder("Temperature: 22°C").build())
    .child(TextBuilder("Humidity: 65%").build())
    .build();

// Layout and render
panel->measure(displayWidth, displayHeight);
panel->layout(0, 0);
panel->draw(tft);
```

## Color System

Predefined colors (16-bit RGB565 format):

```cpp
enum class Color : uint16_t {
    Black       = 0x0000,
    White       = 0xFFFF,
    Red         = 0xF800,
    Green       = 0x07E0,
    Blue        = 0x001F,
    Yellow      = 0xFFE0,
    Cyan        = 0x07FF,
    Magenta     = 0xF81F,
    Orange      = 0xFD20,
    Purple      = 0x8010,
    Gray        = 0x8410,
    DarkGray    = 0x4208,
    LightGray   = 0xC618
};
```

Custom colors:

```cpp
// RGB to RGB565
uint16_t customColor = ((r & 0xF8) << 8) | 
                       ((g & 0xFC) << 3) | 
                       (b >> 3);
```

## Layout System

### Alignment

```cpp
enum class Align {
    LEFT,
    CENTER,
    RIGHT,
    TOP,
    MIDDLE,
    BOTTOM
};
```

### Spacing

- **Padding**: Space inside container
- **Margin**: Space outside component

```cpp
// Container with spacing
auto container = ContainerBuilder()
    .padding(10)    // 10px inside
    .margin(5)      // 5px outside
    .build();
```

### Positioning

Manual positioning:

```cpp
component->layout(x, y);  // Absolute position
```

Automatic positioning (within container):

```cpp
auto container = ContainerBuilder()
    .layout(Layout::VERTICAL)  // Stack children vertically
    .child(component1)
    .child(component2)
    .build();
```

## Display Integration

### TFT_eSPI

The framework uses TFT_eSPI for rendering:

```cpp
#include <TFT_eSPI.h>
#include "libs/common/ui/UI.h"

TFT_eSPI tft;

void setup() {
    tft.init();
    tft.setRotation(1);
    
    // Build UI
    auto ui = buildInterface();
    
    // Render
    ui->measure(tft.width(), tft.height());
    ui->layout(0, 0);
    ui->draw(&tft);
}
```

### Display Configuration

Configure in `platformio.ini`:

```ini
build_flags = 
    -DUSER_SETUP_LOADED=1
    -DST7789_DRIVER=1
    -DTFT_WIDTH=135
    -DTFT_HEIGHT=240
    ; ... more flags
```

## Memory Management

### Smart Pointers

Components use `std::unique_ptr` for automatic memory management:

```cpp
std::unique_ptr<Component> component = 
    TextBuilder("Text").build();
```

### Move Semantics

Ownership transfer using move:

```cpp
auto container = ContainerBuilder()
    .child(std::move(component))  // Transfer ownership
    .build();
```

### RAII

Resources automatically cleaned up:

```cpp
{
    auto component = TextBuilder("Temp").build();
    // ... use component
} // Automatically destroyed here
```

## Performance Optimization

### Minimize Redraws

```cpp
// Only redraw when data changes
if (temperature != lastTemperature) {
    updateTemperatureDisplay();
    lastTemperature = temperature;
}
```

### Partial Updates

```cpp
// Update only changed region
tft.fillRect(x, y, width, height, backgroundColor);
component->draw(&tft);
```

### Double Buffering

```cpp
// Use sprites for flicker-free updates
TFT_eSprite sprite(&tft);
sprite.createSprite(width, height);
component->drawToSprite(&sprite);
sprite.pushSprite(x, y);
sprite.deleteSprite();
```

## Best Practices

### 1. Use Builders

```cpp
// Good: Using builder
auto text = TextBuilder("Hello")
    .fontSize(2)
    .build();

// Avoid: Manual construction
auto text = new Text("Hello");
text->setFontSize(2);
```

### 2. Proper Layout

```cpp
// Always measure before layout
component->measure(width, height);
component->layout(x, y);
component->draw(display);
```

### 3. Smart Pointers

```cpp
// Good: Use unique_ptr
std::unique_ptr<Component> component = builder.build();

// Avoid: Raw pointers
Component* component = builder.buildRaw();
```

### 4. Const Correctness

```cpp
void renderComponent(const Component& component) {
    // Read-only access
}
```

## Testing

UI components can be tested on native platforms without hardware:

```cpp
#include "test/mocks/MockDisplay.h"

void test_text_component() {
    MockDisplay display;
    
    auto text = TextBuilder("Test")
        .fontSize(2)
        .build();
    
    text->measure(100, 50);
    text->layout(10, 10);
    text->draw(&display);
    
    TEST_ASSERT_TRUE(display.wasDrawn());
}
```

See [Snapshot Testing](/development/ui/snapshot-testing) for detailed testing guide.

## Examples

### Simple Status Display

```cpp
void displayStatus(float temp, float humidity) {
    tft.fillScreen(TFT_BLACK);
    
    auto ui = ContainerBuilder()
        .backgroundColor(Color::DarkGray)
        .padding(10)
        .child(
            TextBuilder("Plant Status")
                .fontSize(3)
                .color(Color::Green)
                .align(Align::CENTER)
                .build()
        )
        .child(
            TextBuilder(String("Temp: ") + temp + "°C")
                .fontSize(2)
                .color(Color::White)
                .build()
        )
        .child(
            TextBuilder(String("Humidity: ") + humidity + "%")
                .fontSize(2)
                .color(Color::Cyan)
                .build()
        )
        .build();
    
    ui->measure(tft.width(), tft.height());
    ui->layout(0, 0);
    ui->draw(&tft);
}
```

### Warning Alert

```cpp
void showWarning(const String& message) {
    auto alert = ContainerBuilder()
        .backgroundColor(Color::Red)
        .padding(15)
        .border(true, Color::White)
        .borderWidth(3)
        .child(
            TextBuilder("⚠ WARNING")
                .fontSize(3)
                .color(Color::Yellow)
                .bold(true)
                .align(Align::CENTER)
                .build()
        )
        .child(
            TextBuilder(message)
                .fontSize(2)
                .color(Color::White)
                .align(Align::CENTER)
                .build()
        )
        .build();
    
    alert->measure(tft.width() - 20, 100);
    alert->layout(10, tft.height()/2 - 50);
    alert->draw(&tft);
}
```

## Next Steps

- [UI Components](/development/ui/components) - Detailed component reference
- [Snapshot Testing](/development/ui/snapshot-testing) - Test your UI
- [Quick Reference](/development/ui/snapshot-testing-quick-ref) - Cheat sheet
