# UI Components Reference

> **Note:** This page is under construction. See [UI Framework](/development/ui/framework) for general UI information.

Complete reference for all available UI components in the PlantNanny UI framework.

## Text Component

Display text with customizable styling.

### Constructor

```cpp
TextBuilder(const String& text)
```

### Methods

```cpp
TextBuilder& fontSize(int size);              // 1-8
TextBuilder& color(Color textColor);
TextBuilder& backgroundColor(Color bgColor);
TextBuilder& align(Align alignment);          // LEFT, CENTER, RIGHT
TextBuilder& underline(bool enabled);
TextBuilder& bold(bool enabled);
std::unique_ptr<Text> build();
```

### Example

```cpp
auto text = TextBuilder("Hello World")
    .fontSize(2)
    .color(Color::White)
    .backgroundColor(Color::Black)
    .align(Align::CENTER)
    .underline(true)
    .build();
```

### Properties

- **Text**: String content
- **Font Size**: 1-8 (maps to TFT_eSPI fonts)
- **Color**: 16-bit RGB565 color
- **Background**: Optional background color
- **Alignment**: LEFT, CENTER, RIGHT
- **Underline**: Boolean
- **Bold**: Boolean (simulated via double-draw)

## Container Component

Group and layout child components.

### Constructor

```cpp
ContainerBuilder()
```

### Methods

```cpp
ContainerBuilder& padding(int pixels);
ContainerBuilder& margin(int pixels);
ContainerBuilder& backgroundColor(Color color);
ContainerBuilder& border(bool enabled, Color color = Color::White);
ContainerBuilder& borderWidth(int width);
ContainerBuilder& child(std::unique_ptr<Component> component);
ContainerBuilder& layout(Layout layoutType);  // VERTICAL, HORIZONTAL
std::unique_ptr<Container> build();
```

### Example

```cpp
auto container = ContainerBuilder()
    .padding(10)
    .margin(5)
    .backgroundColor(Color::DarkGray)
    .border(true, Color::White)
    .borderWidth(2)
    .child(TextBuilder("Title").fontSize(3).build())
    .child(TextBuilder("Content").fontSize(2).build())
    .layout(Layout::VERTICAL)
    .build();
```

### Properties

- **Padding**: Internal spacing (all sides)
- **Margin**: External spacing (all sides)
- **Background**: Fill color
- **Border**: Optional border with color and width
- **Children**: List of child components
- **Layout**: VERTICAL (stack) or HORIZONTAL (row)

## Button Component (Planned)

Interactive button with callback.

### Example (Future API)

```cpp
auto button = ButtonBuilder("Click Me")
    .fontSize(2)
    .backgroundColor(Color::Green)
    .textColor(Color::White)
    .onPress([]() {
        Serial.println("Button pressed!");
    })
    .build();
```

## Image Component (Planned)

Display images and icons.

### Example (Future API)

```cpp
auto image = ImageBuilder()
    .source("/images/icon.png")
    .width(64)
    .height(64)
    .build();
```

## List Component (Planned)

Scrollable list of items.

### Example (Future API)

```cpp
auto list = ListBuilder()
    .addItem(TextBuilder("Item 1").build())
    .addItem(TextBuilder("Item 2").build())
    .scrollable(true)
    .build();
```

## Component Base Class

All components inherit from the base `Component` interface:

```cpp
class Component {
public:
    virtual ~Component() = default;
    
    // Layout methods
    virtual void measure(int availableWidth, int availableHeight) = 0;
    virtual void layout(int x, int y) = 0;
    virtual void draw(TFT_eSPI* display) = 0;
    
    // Getters
    int getWidth() const;
    int getHeight() const;
    int getX() const;
    int getY() const;
    
protected:
    int width_ = 0;
    int height_ = 0;
    int x_ = 0;
    int y_ = 0;
};
```

## Common Patterns

### Nested Containers

```cpp
auto header = ContainerBuilder()
    .backgroundColor(Color::Blue)
    .padding(10)
    .child(TextBuilder("Header").fontSize(3).build())
    .build();

auto content = ContainerBuilder()
    .backgroundColor(Color::DarkGray)
    .padding(10)
    .child(TextBuilder("Content").build())
    .build();

auto page = ContainerBuilder()
    .layout(Layout::VERTICAL)
    .child(std::move(header))
    .child(std::move(content))
    .build();
```

### Styled Text

```cpp
auto title = TextBuilder("PlantNanny")
    .fontSize(4)
    .color(Color::Green)
    .bold(true)
    .align(Align::CENTER)
    .build();

auto subtitle = TextBuilder("Smart Plant Care")
    .fontSize(2)
    .color(Color::Gray)
    .align(Align::CENTER)
    .build();
```

### Status Display

```cpp
auto statusDisplay = ContainerBuilder()
    .padding(5)
    .border(true, Color::White)
    .child(TextBuilder("Temperature: 22°C")
        .color(Color::Red)
        .build())
    .child(TextBuilder("Humidity: 55%")
        .color(Color::Blue)
        .build())
    .child(TextBuilder("Light: High")
        .color(Color::Yellow)
        .build())
    .layout(Layout::VERTICAL)
    .build();
```

## Color Reference

See [UI Framework - Color System](/development/ui/framework#color-system) for available colors.

## Testing Components

All components support snapshot testing:

```cpp
void test_text_component() {
    MockDisplay display;
    
    auto text = TextBuilder("Test")
        .fontSize(2)
        .color(Color::White)
        .build();
    
    text->measure(100, 50);
    text->layout(10, 10);
    text->draw(&display);
    
    TEST_ASSERT_TRUE(assertSnapshot("text_test"));
}
```

See [Snapshot Testing](/development/ui/snapshot-testing) for details.

## Next Steps

- [UI Framework](/development/ui/framework) - Framework overview
- [Snapshot Testing](/development/ui/snapshot-testing) - Test your UI
- [Quick Reference](/development/ui/snapshot-testing-quick-ref) - Cheat sheet

---

*This page will be expanded as more components are added. Check back for updates!*
