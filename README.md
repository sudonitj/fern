# Fern Graphics Library

<p align="center">
  <img src="assets/logo.png" alt="Fern Graphics Logo" width="200"/>
</p>

A lightweight single-file WebAssembly-based graphics library for creating interactive visual applications using simple C code.

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [Requirements](#requirements)
- [Installation](#installation)
  - [Step 1: Install Dependencies](#step-1-install-dependencies)
  - [Step 2: Install Fern CLI](#step-2-install-fern-cli)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
  - [Core Types](#core-types)
  - [Color Constants](#color-constants)
  - [Widget Functions](#widget-functions)
  - [Core Drawing Functions](#core-drawing-functions)
  - [Application Lifecycle](#application-lifecycle)
  - [PPM Export](#ppm-export)
- [Advanced Techniques](#advanced-techniques)
  - [Creating Filled Shapes](#creating-filled-shapes)
  - [Creating Scene Components](#creating-scene-components)
- [CLI Tool Usage](#cli-tool-usage)
- [Architecture](#architecture)
- [Contributing](#contributing)
- [License](#license)

## Overview

Fern is a minimalist graphics library designed for simplicity, performance, and ease of use. It provides a declarative API for rendering graphics to HTML canvas via WebAssembly, enabling C developers to create visual applications that run in any modern web browser.

## Key Features

- Single-file implementation for easy integration
- Declarative API with named parameters
- WebAssembly-powered rendering for near-native performance
- Support for basic shapes, lines, and pixel manipulation
- Simple CLI tool for compiling and serving applications
- PPM image export capability for saving renderings
- Lightweight (~500 lines of code)

## Requirements

- Emscripten SDK
- Modern C compiler (supporting C99 or later)
- Python 3 (for development server)
- Web browser with WebAssembly support

## Installation

### Step 1: Install Dependencies

```bash
# Install Emscripten SDK
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

### Step 2: Install Fern CLI

```bash
# Clone the repository
git clone https://github.com/yourusername/fern.git
cd fern

# Make the CLI script executable
chmod +x fern-cli.sh

# Create a symbolic link to make it available system-wide
sudo ln -s $(pwd)/fern-cli.sh /usr/local/bin/fern

# Optional: Install the man page
sudo install -m 644 fern.1 /usr/local/share/man/man1/
sudo mandb
```

## Quick Start

Create a basic application:

```c
// main.c
#include "fern.c"

#define WIDTH 800
#define HEIGHT 600

static uint32_t pixels[HEIGHT*WIDTH];

int main() {
    FernCanvas canvas = {pixels, HEIGHT, WIDTH};
    runApp(canvas);
    
    Container(
        color(Colors_blue),
        x(0),
        y(0),
        width(WIDTH),
        height(HEIGHT)
    );
    
    CircleWidget(
        radius(50),
        position(Point_create(WIDTH/2, HEIGHT/2)),
        color(Colors_red)
    );
    
    fern_start_render_loop();
    return 0;
}
```

Compile and run:

```bash
fern main.c
# Open http://localhost:8000/dist/ in your browser
```

## API Reference

### Core Types

#### `Point`
Represents a 2D point with x and y coordinates.

```c
struct Point {
    int x;
    int y;
};

// Create a Point
Point Point_create(int x, int y);
```

#### `FernCanvas`
Represents the drawing canvas.

```c
struct FernCanvas {
    uint32_t* pixels;
    size_t height;
    size_t width;
};
```

### Color Constants

Fern provides predefined color constants:

```c
#define Colors_green 0xFF00FF00   // Green
#define Colors_blue  0xFF0000FF   // Blue
#define Colors_red   0xFFFF0000   // Red
#define Colors_gray  0xFF202020   // Dark Gray
#define Colors_black 0xFF000000   // Black
#define Colors_white 0xFFFFFFFF   // White
```

Color format is 0xAARRGGBB (alpha, red, green, blue).

### Widget Functions

#### Container
Creates a rectangular container.

```c
Container(
    color(uint32_t color),
    x(int x),
    y(int y),
    width(int width),
    height(int height)
);
```

#### CenteredContainer
Creates a centered rectangular container.

```c
CenteredContainer(
    width(int width),
    height(int height),
    color(uint32_t color)
);
```

#### CircleWidget
Creates a circle.

```c
CircleWidget(
    radius(int radius),
    position(Point position),
    color(uint32_t color)
);
```

#### LineWidget
Creates a line with specified thickness.

```c
LineWidget(
    start(Point start),
    end(Point end),
    thickness(int thickness),
    color(uint32_t color)
);
```

### Core Drawing Functions

For more advanced use cases, you can use the lower-level drawing functions:

```c
// Fill entire canvas with a color
void ffill(uint32_t* pixels, size_t height, size_t width, uint32_t color);

// Draw a rectangle
void frect(uint32_t* pixels, size_t height, size_t width, uint32_t color, 
          size_t x, size_t y, size_t w, size_t h);

// Draw a circle
void fcircle(uint32_t* pixels, size_t height, size_t width, uint32_t color, 
            size_t cx, size_t cy, size_t r);

// Draw a line with thickness
void fline(uint32_t* pixels, size_t height, size_t width, uint32_t color, 
          int x1, int y1, int x2, int y2, int thickness);
```

### Application Lifecycle

```c
// Initialize the application with a canvas
void runApp(FernCanvas canvas);

// Start the rendering loop
void fern_start_render_loop(void);
```

### PPM Export

The library provides a function to export renders as PPM images:

```c
// Save the current pixel buffer as a PPM file
int fsave_ppm(uint32_t* pixels, size_t width, size_t height, const char* filename);

// Example usage
fsave_ppm(canvas.pixels, canvas.width, canvas.height, "output.ppm");
```

## Advanced Techniques

### Creating Filled Shapes

To create filled triangle shapes:

```c
// Fill a triangle using fan pattern
for (int i = 0; i < 180; i++) {
    int baseX = leftX + i * (rightX-leftX)/180;
    
    LineWidget(
        start(Point_create(baseX, baseY)),  
        end(Point_create(peakX, peakY)), 
        thickness(1),
        color(Colors_gray)  
    );
}
```

### Creating Scene Components

You can create reusable scene components by defining functions:

```c
// Draw a cloud at the specified position
void drawCloud(int x, int y, int size) {
    CircleWidget(
        radius(size),
        position(Point_create(x, y)),
        color(Colors_white)
    );
    
    CircleWidget(
        radius(size * 1.2),
        position(Point_create(x + size, y - 5)),
        color(Colors_white)
    );
    
    CircleWidget(
        radius(size),
        position(Point_create(x + size*2, y)),
        color(Colors_white)
    );
}
```

## CLI Tool Usage

```
fern [FILENAME]
```

- If FILENAME is not provided, looks for main.c or example.c in the current directory
- Compiles the specified file to WebAssembly
- Creates a dist/ directory if it doesn't exist
- Starts a local web server
- Open http://localhost:8000/dist/ in your browser

## Architecture

### Library Structure

```mermaid
graph TD
    A[main.c: User Application] --> B[fern.c: Graphics Library]
    B --> C[WebAssembly Module]
    C --> D[Canvas Rendering]
    
    subgraph "Core Components"
        E[Drawing Functions] --> F[Pixel Buffer]
        G[Widget System] --> E
        H[Render Loop] --> F
        B --> E
        B --> G
        B --> H
    end
    
    subgraph "Build System"
        I[fern-cli.sh] --> J[Emscripten Compiler]
        J --> C
        J --> K[HTML/JS Shell]
        K --> L[Browser Display]
    end
    
    F --> M[PPM Export]
```

### Rendering Pipeline

```mermaid
sequenceDiagram
    participant User as User Code
    participant Fern as Fern Library
    participant WASM as WebAssembly
    participant Canvas as HTML Canvas
    
    User->>Fern: Create Canvas
    User->>Fern: Draw Widgets
    Fern->>Fern: Update Pixel Buffer
    User->>Fern: Start Render Loop
    
    loop Every Frame
        Fern->>WASM: Pass Pixel Buffer
        WASM->>Canvas: Update ImageData
        Canvas->>User: Display Frame
    end
```

## Example

![Fern Graphics Example](assets/example_scene.png)

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
