#pragma once

#include "types.hpp"
#include <cstdint>

namespace Fern {
    class Canvas {
    public:
        Canvas(uint32_t* buffer, int width, int height);
        
        void clear(uint32_t color);
        void setPixel(int x, int y, uint32_t color);
        uint32_t getPixel(int x, int y) const;
        
        int getWidth() const { return width_; }
        int getHeight() const { return height_; }
        uint32_t* getBuffer() const { return buffer_; }
        
    private:
        uint32_t* buffer_;
        int width_;
        int height_;
    };
    
    // Global canvas instance
    extern Canvas* globalCanvas;
}