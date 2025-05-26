#include "../../include/fern/core/canvas.hpp"
#include <cstring>

namespace Fern {
    Canvas* globalCanvas = nullptr;
    
    Canvas::Canvas(uint32_t* buffer, int width, int height)
        : buffer_(buffer), width_(width), height_(height) {}
    
    void Canvas::clear(uint32_t color) {
        for (int i = 0; i < width_ * height_; ++i) {
            buffer_[i] = color;
        }
    }
    
    void Canvas::setPixel(int x, int y, uint32_t color) {
        if (x >= 0 && x < width_ && y >= 0 && y < height_) {
            buffer_[y * width_ + x] = color;
        }
    }
    
    uint32_t Canvas::getPixel(int x, int y) const {
        if (x >= 0 && x < width_ && y >= 0 && y < height_) {
            return buffer_[y * width_ + x];
        }
        return 0;
    }
}