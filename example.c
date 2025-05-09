#include "fern.c"
#include <stdio.h>

#define WIDTH 800
#define HEIGHT 600

static uint32_t pixels [HEIGHT*WIDTH];

int main () {
    ffill(pixels, HEIGHT, WIDTH, 0xFF0000FF); // Fill with red
    fsave_ppm(pixels, WIDTH, HEIGHT, "fern.ppm");
    return 0;
}