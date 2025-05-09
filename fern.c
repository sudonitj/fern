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

#endif // FERN_H_