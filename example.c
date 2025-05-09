#include "fern.c"
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600

static uint32_t pixels [HEIGHT*WIDTH];

void fline_ineff(uint32_t* pixels, size_t px_height, size_t px_width, uint32_t color, int x1, int y1, int x2, int y2) {
    float m = (y2 - y1) / (x2 - x1);

    for (int y = 0; y < px_height; y++){
        for (int x = 0; x < px_width; x++){
            if((x1-x2) !=0){
                if((y - y1) == m * (x - x1)){
                    pixels[y * px_width + x] = color;
                }
            }else{

            }
        }
        
    }
}



int main () {
    int rw = 200;
    int rh = 100;
    int r = 50;
    ffill(pixels, HEIGHT, WIDTH, 0xFF202020);
    frect(pixels, HEIGHT, WIDTH, 0xFF00FF00, WIDTH/2 - rw/2, HEIGHT/2 - rh/2, rw, rh);
    fcircle(pixels, HEIGHT, WIDTH, 0x00000FF, r, r, r);
    fline(pixels, HEIGHT, WIDTH, 0xFFFF0000, 10, 10, 50, 50, 3);
    fsave_ppm(pixels, WIDTH, HEIGHT, "fern.ppm");
    return 0;
}