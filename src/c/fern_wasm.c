#include "fern.c" 
#include <emscripten.h>

#define CANVAS_WIDTH 800
#define CANVAS_HEIGHT 600

static uint32_t canvas_pixels[CANVAS_HEIGHT * CANVAS_WIDTH];

EMSCRIPTEN_KEEPALIVE
void init_canvas() {
    ffill(canvas_pixels, CANVAS_HEIGHT, CANVAS_WIDTH, 0xFF202020);
}

EMSCRIPTEN_KEEPALIVE
void draw_rect(int x, int y, int w, int h, uint32_t color) {
    frect(canvas_pixels, CANVAS_HEIGHT, CANVAS_WIDTH, color, x, y, w, h);
}

// Draw a circle
EMSCRIPTEN_KEEPALIVE
void draw_circle(int cx, int cy, int r, uint32_t color) {
    fcircle(canvas_pixels, CANVAS_HEIGHT, CANVAS_WIDTH, color, cx, cy, r);
}

// Draw a line
EMSCRIPTEN_KEEPALIVE
void draw_line(int x1, int y1, int x2, int y2, int thickness, uint32_t color) {
    fline(canvas_pixels, CANVAS_HEIGHT, CANVAS_WIDTH, color, x1, y1, x2, y2, thickness);
}

// Get the pixel buffer data
EMSCRIPTEN_KEEPALIVE
uint8_t* get_pixel_data() {
    // Return a pointer to the raw pixel data
    return (uint8_t*)canvas_pixels;
}