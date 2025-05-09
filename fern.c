#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <emscripten.h>

typedef struct Point Point;
typedef struct FernCanvas FernCanvas;

struct Point {
    int x;
    int y;
};

struct FernCanvas {
    uint32_t* pixels;
    size_t height;
    size_t width;
};

#define Colors_green 0xFF00FF00
#define Colors_blue  0xFF0000FF
#define Colors_red   0xFFFF0000
#define Colors_gray  0xFF202020
#define Colors_black 0xFF000000
#define Colors_white 0xFFFFFFFF

void fern_init_wasm(uint32_t* pixel_buffer, size_t height, size_t width);
void ffill(uint32_t* pixels, size_t height, size_t width, uint32_t color);
void frect(uint32_t* pixels, size_t height, size_t width, uint32_t color, 
          size_t x, size_t y, size_t w, size_t h);
void fcircle(uint32_t* pixels, size_t height, size_t width, uint32_t color, 
            size_t cx, size_t cy, size_t r);
void fline(uint32_t* pixels, size_t height, size_t width, uint32_t color, 
          int x1, int y1, int x2, int y2, int thickness);
void fern_start_render_loop(void);
Point Point_create(int x, int y);
void runApp(FernCanvas canvas);
void Container(uint32_t color, int x, int y, int width, int height);
void CenteredContainer(int width, int height, uint32_t color);
void CircleWidget(int radius, Point position, uint32_t color);
void LineWidget(Point start, Point end, int thickness, uint32_t color);

#define color(c) c
#define x(val) val
#define y(val) val
#define width(w) w
#define height(h) h
#define radius(r) r
#define position(p) p
#define start(p) p
#define end(p) p
#define thickness(t) t

static uint32_t* buffer_ptr = NULL;
static size_t buffer_width = 0;
static size_t buffer_height = 0;
FernCanvas current_canvas;

#define return_defer(value) do {result = (value); goto defer;} while(0)
typedef int Errno;

Point Point_create(int x, int y) {
    Point p = {x, y};
    return p;
}

void runApp(FernCanvas canvas) {
    current_canvas = canvas;
    fern_init_wasm(canvas.pixels, canvas.height, canvas.width);    
    ffill(canvas.pixels, canvas.height, canvas.width, Colors_gray);
}

void Container(uint32_t color, int x, int y, int width, int height) {
    frect(current_canvas.pixels, current_canvas.height, current_canvas.width,
          color, x, y, width, height);
}

void CenteredContainer(int width, int height, uint32_t color) {
    int centered_x = (current_canvas.width - width) / 2;
    int centered_y = (current_canvas.height - height) / 2;    
    Container(color, centered_x, centered_y, width, height);
}

void CircleWidget(int radius, Point position, uint32_t color) {
    fcircle(current_canvas.pixels, current_canvas.height, current_canvas.width,
            color, position.x, position.y, radius);
}

void LineWidget(Point start, Point end, int thickness, uint32_t color) {
    fline(current_canvas.pixels, current_canvas.height, current_canvas.width,
          color, start.x, start.y, end.x, end.y, thickness);
}

static void internal_render_loop() {
    if (!buffer_ptr) return;
    
    EM_ASM({
        var canvas = document.getElementById('canvas');
        var ctx = canvas.getContext('2d');        
        if (canvas.width !== $1 || canvas.height !== $0) {
            canvas.width = $1;  
            canvas.height = $0; 
        }
        var imageData = ctx.createImageData(canvas.width, canvas.height);
        
        var data = imageData.data;
        var buffer = $2; 
        var size = $0 * $1;
        
        for (var i = 0; i < size; i++) {
            var pixel = HEAP32[buffer/4 + i]; 
            var r = pixel & 0xFF;
            var g = (pixel >> 8) & 0xFF;
            var b = (pixel >> 16) & 0xFF;
            var a = (pixel >> 24) & 0xFF;            
            var j = i * 4;
            data[j + 0] = r;
            data[j + 1] = g;
            data[j + 2] = b;
            data[j + 3] = a;
        }
        
        ctx.putImageData(imageData, 0, 0);       
    }, buffer_height, buffer_width, buffer_ptr);
}

void fern_init_wasm(uint32_t* pixel_buffer, size_t height, size_t width) {
    buffer_ptr = pixel_buffer;
    buffer_height = height;
    buffer_width = width;
}

void fern_start_render_loop(void) {
    emscripten_set_main_loop(internal_render_loop, 0, 1);
}

void ffill(uint32_t* pixels, size_t height, size_t width, uint32_t color){
    for(size_t i = 0; i < height*width; ++i){
        pixels[i] = color;
    }
}

Errno fsave_ppm(uint32_t* pixels, size_t width, size_t height, const char* filename){
    int result = 0;
    FILE* file = NULL;

    {
        file = fopen(filename, "wb");
        if (!file) return_defer(errno);
        fprintf(file, "P6\n%zu %zu\n255\n", width, height);
        if(ferror(file)) return_defer(errno);

        for (size_t i = 0; i < width*height; i++){
            // 0xAABBGGRR
            uint32_t pixel = pixels[i];
            uint8_t bytes[3] = {
                pixel&0xFF,
                (pixel>>8)&0xFF,
                (pixel>>16)&0xFF
            };

            fwrite(bytes, sizeof(bytes), 1, file);
            if(ferror(file)) return_defer(errno);
        }
        
    }
    defer: 
    if(file) fclose(file);
    return result;
}

void frect(uint32_t* pixels, size_t height, size_t width, uint32_t color, size_t x, size_t y, size_t w, size_t h) {
    for (int dx = 0; dx < (int) h; ++dx) {
        for (int dy = 0; dy < (int) w; ++dy) {
            if (x + dy < width && y + dx < height) {
                pixels[(y + dx) * width + (x + dy)] = color;
            }
        }
    }
}

void fcircle(uint32_t* pixels, size_t height, size_t width, uint32_t color, size_t cx, size_t cy, size_t r) {
    for (int x = -r; x <= (int)r; x++){
        for(int y = -r; y <= (int) r; y++){
            if (x*x + y*y <= (int) (r*r)){
                int px = cx + x;
                int py = cy + y;
                if (px >= 0 && px < (int)width && py >= 0 && py < (int)height){
                    pixels[py * width + px] = color;
                }
            }
        }
    }
}

// based on bresenham's algo
void fline(uint32_t* pixels, size_t px_height, size_t px_width, uint32_t color, int x1, int y1, int x2, int y2, int thickness){
    int t = thickness;

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;

    int err = (dx - dy);

    while(1){
        if (x1 >= 0 && x1 < px_width && y1 >= 0 && y1 < px_height) {
            // pixels[y1 * px_width + x1] = color; 
            fcircle(pixels, px_height, px_width, color, x1, y1, t);
        }

        if(x1 == x2 && y1 == y2) break;
        if(err > -dy){
            err -= dy;
            x1 += sx;
        }

        if(err < dx) {
            err += dx;
            y1 += sy;
        }
    }
}