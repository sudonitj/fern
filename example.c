#include "fern.c"
#include <stdio.h>

#define WIDTH 1200
#define HEIGHT 600

static uint32_t pixels [HEIGHT*WIDTH];


int main () {
    int rw = 200;
    int rh = 100;
    int r = 50;

    fern_init_wasm(pixels, HEIGHT, WIDTH);

    ffill(pixels, HEIGHT, WIDTH, 0xFF202020);
    frect(pixels, HEIGHT, WIDTH, 0xFF00FF00, WIDTH/2 - rw/2, HEIGHT/2 - rh/2, rw, rh);
    fcircle(pixels, HEIGHT, WIDTH, 0xFF0000FF, r, r, r);
    fline(pixels, HEIGHT, WIDTH, 0xFFFF0000, 10, 10, 50, 50, 3);
    // fsave_ppm(pixels, WIDTH, HEIGHT, "fern.ppm");
    fern_start_render_loop();
    return 0;
}