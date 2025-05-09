# Fern

A lightweight graphics manipulation library written in C.

## Overview

Fern is a minimalist C library for pixel manipulation and image generation. Currently in its initial development stage, it provides basic functionality for creating and saving simple graphics.

## Features

- Pixel buffer manipulation
- PPM image file output
- Simple error handling

## Installation

Clone the repository:
```
git clone https://github.com/yourusername/fern.git
cd fern
```

## Usage

```c
#include "fern.c"

#define WIDTH 800
#define HEIGHT 600

static uint32_t pixels[HEIGHT*WIDTH];

int main() {
    // Fill the pixel buffer with red (0xAABBGGRR format)
    ffill(pixels, HEIGHT, WIDTH, 0xFF0000FF);
    
    // Save the pixels to a PPM file
    fsave_ppm(pixels, WIDTH, HEIGHT, "output.ppm");
    
    return 0;
}
```

## Building Example

```bash
cc -Wall -o example example.c
./example
```

## API Reference

### Functions

#### `void ffill(uint32_t* pixels, size_t height, size_t width, uint32_t color)`
Fills a pixel buffer with a specified color.

- `pixels`: Pointer to the pixel buffer
- `height`: Height of the pixel buffer
- `width`: Width of the pixel buffer
- `color`: Color value in 0xAABBGGRR format

#### `Errno fsave_ppm(uint32_t* pixels, size_t width, size_t height, const char* filename)`
Saves a pixel buffer to a PPM file.

- `pixels`: Pointer to the pixel buffer
- `width`: Width of the pixel buffer
- `height`: Height of the pixel buffer
- `filename`: Output filename
- Returns: 0 on success, error code on failure

## Color Format

Fern uses 32-bit ARGB color values in the format `0xAABBGGRR`:
- `AA`: Alpha channel (currently ignored in output)
- `BB`: Blue channel
- `GG`: Green channel
- `RR`: Red channel

## Future Plans

- Support for drawing shapes and lines
- Additional file formats
- Transformation operations
- Text rendering
- Advanced color management

## License

MIT License

## Contributing

This project is in its early stages. Contributions, suggestions, and feedback are welcome.