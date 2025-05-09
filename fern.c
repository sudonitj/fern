#ifndef FERN_H
#define FERN_H

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <emscripten.h>

static uint32_t* buffer_ptr = NULL;
static size_t buffer_width = 0;
static size_t buffer_height = 0;

#define return_defer(value) do {result = (value); goto defer;} while(0)

typedef int Errno;


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
                if (px < (int)width && py < (int)height){
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

#endif // FERN_H_