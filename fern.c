#ifndef FERN_H_
#define FERN_H_

#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#define return_defer(value) do {result = (value); goto defer;} while(0)

typedef int Errno;

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