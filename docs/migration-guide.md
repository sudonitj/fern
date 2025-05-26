# Fern Graphics: C to C++ Migration Guide

This guide provides instructions for transitioning from the C implementation of Fern Graphics to the newer C++ implementation.

> **Important**: The C++ implementation is currently in development and not all features from the C implementation have been ported yet.

## Table of Contents
- [Overview](#overview)
- [Key Differences](#key-differences)
- [API Comparison](#api-comparison)
- [Step-by-Step Migration](#step-by-step-migration)
- [Common Patterns](#common-patterns)
- [Known Issues](#known-issues)

## Overview

The C++ implementation of Fern Graphics offers several advantages over the original C implementation:

- **Better organization**: Split into multiple files with clear separation of concerns
- **Proper encapsulation**: Uses classes and namespaces to prevent naming conflicts
- **Modern C++ features**: Including standard containers, RAII, and better type safety
- **Extensibility**: Easier to extend through inheritance and composition
- **Error checking**: More robust error handling and validation

## Key Differences

### Include Pattern

**C Implementation:**
```c
#include "src/c/fern.c"
```

**C++ Implementation:**
```cpp
#include <fern/fern.hpp>
```

### Application Structure

**C Implementation:**
```c
static uint32_t pixels[HEIGHT*WIDTH];

int main() {
    FernCanvas canvas = {pixels, HEIGHT, WIDTH};
    runApp(canvas);
    
    // Set draw callback
    fern_set_draw_callback(draw_frame);
    
    // Start rendering
    fern_start_render_loop();
    return 0;
}
```

**C++ Implementation:**
```cpp
static uint32_t pixels[HEIGHT*WIDTH];

int main() {
    // Initialize
    Fern::initialize(pixels, WIDTH, HEIGHT);
    
    // Set draw callback
    Fern::setDrawCallback(draw_frame);
    
    // Start rendering
    Fern::startRenderLoop();
    return 0;
}
```

### Drawing Functions

**C Implementation:**
```c
// Direct drawing functions
ffill(pixels, height, width, Colors_blue);
frect(pixels, height, width, Colors_red, 10, 20, 100, 50);

// Widget-style API
Container(
    color(Colors_blue),
    x(0), y(0),
    width(WIDTH), height(HEIGHT)
);

CircleWidget(
    radius(50),
    position(Point_create(WIDTH/2, HEIGHT/2)),
    color(Colors_red)
);
```

**C++ Implementation:**
```cpp
// Namespace-based API
Fern::Draw::fill(Colors::Blue);
Fern::Draw::rect(10, 20, 100, 50, Colors::Red);

// Widget API
Fern::Container(
    Fern::Colors::Blue,
    0, 0, WIDTH, HEIGHT
);

Fern::CircleWidget(
    50,
    Fern::Point(WIDTH/2, HEIGHT/2),
    Fern::Colors::Red
);
```

## API Comparison

| C Function/Type | C++ Equivalent | Notes |
|-----------------|----------------|-------|
| `Point_create(x, y)` | `Fern::Point(x, y)` | Constructor-based creation |
| `FernCanvas` | `Fern::Canvas` | Uses class instead of struct |
| `runApp(canvas)` | `Fern::initialize(pixels, width, height)` | Global canvas instance used internally |
| `fern_set_draw_callback(func)` | `Fern::setDrawCallback(func)` | Uses std::function |
| `fern_start_render_loop()` | `Fern::startRenderLoop()` | Same behavior |
| `ButtonWidget(config)` | `Fern::ButtonWidget(config)` | Similar API, different struct layout |
| `Colors_red` | `Fern::Colors::Red` | Namespace-based constants |
| `current_input` | `Fern::Input::getState()` | Access through function |
| `fernPrintf(msg)` | `Fern::Console::print(msg)` | Enhanced console functions |

## Step-by-Step Migration

1. **Include headers** - Replace the C include with the C++ header:
   ```cpp
   // From
   #include "src/c/fern.c"
   
   // To
   #include <fern/fern.hpp>
   ```

2. **Update initialization** - Replace the `runApp()` call:
   ```cpp
   // From
   FernCanvas canvas = {pixels, HEIGHT, WIDTH};
   runApp(canvas);
   
   // To
   Fern::initialize(pixels, WIDTH, HEIGHT);
   ```

3. **Update function calls** - Add the `Fern::` namespace prefix to all functions:
   ```cpp
   // From
   CircleWidget(radius(50), position(Point_create(100, 100)), color(Colors_red));
   
   // To
   Fern::CircleWidget(50, Fern::Point(100, 100), Fern::Colors::Red);
   ```

4. **Update callbacks** - Keep the same signature but update function body as needed:
   ```cpp
   // From
   void draw_frame(void) {
       Container(color(Colors_blue), x(0), y(0), width(WIDTH), height(HEIGHT));
   }
   fern_set_draw_callback(draw_frame);
   
   // To
   void draw_frame() {
       Fern::Draw::fill(Fern::Colors::Blue);
   }
   Fern::setDrawCallback(draw_frame);
   ```

5. **Update input handling** - Use the Input class:
   ```cpp
   // From
   int mouseX = current_input.mouse_x;
   bool isClicked = current_input.mouse_clicked;
   
   // To
   const auto& input = Fern::Input::getState();
   int mouseX = input.mouseX;
   bool isClicked = input.mouseClicked;
   ```

## Common Patterns

### Creating Buttons

**C Implementation:**
```c
void button_callback() {
    // Handle click
}

ButtonConfig btn = {
    .x = 100,
    .y = 100,
    .width = 200,
    .height = 50,
    .normal_color = Colors_blue,
    .hover_color = 0xFF4444FF,
    .press_color = 0xFF0000AA,
    .label = "CLICK ME",
    .text_scale = 2,
    .text_color = Colors_white,
    .on_click = button_callback
};

ButtonWidget(btn);
```

**C++ Implementation:**
```cpp
auto button_callback = []() {
    // Handle click with lambda
};

Fern::ButtonConfig btn = {
    .x = 100,
    .y = 100,
    .width = 200,
    .height = 50,
    .normalColor = Fern::Colors::Blue,
    .hoverColor = 0xFF4444FF,
    .pressColor = 0xFF0000AA,
    .label = "CLICK ME",
    .textScale = 2,
    .textColor = Fern::Colors::White,
    .onClick = button_callback
};

Fern::ButtonWidget(btn);
```

### Custom Drawing

**C Implementation:**
```c
// Direct pixel manipulation
for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
        pixels[y * WIDTH + x] = (x ^ y) * 0xFF;
    }
}
```

**C++ Implementation:**
```cpp
// Using the Canvas class
auto* canvas = Fern::globalCanvas;
for (int y = 0; y < canvas->getHeight(); y++) {
    for (int x = 0; x < canvas->getWidth(); x++) {
        canvas->setPixel(x, y, (x ^ y) * 0xFF);
    }
}
```

## Known Issues

1. **Incomplete implementation** - Not all features from the C implementation are available yet
2. **API changes** - The API is still evolving and may change before final release
3. **Performance differences** - The C++ implementation may have different performance characteristics 
4. **Integration with existing code** - Mixing C and C++ code might require additional boilerplate

## Timeline

The C++ implementation is under active development. Here's the current timeline:

- **Phase 1** (Current): Core rendering, basic shapes, and simple UI elements
- **Phase 2**: Complete UI system, text rendering, and event handling
- **Phase 3**: Advanced features, optimizations, and full API compatibility

We welcome contributions to the C++ implementation! See CONTRIBUTING.md for details on how to contribute.
