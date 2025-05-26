#pragma once

#include <cstdint>

namespace Fern {
    namespace Colors {
        constexpr uint32_t Green = 0xFF00FF00;
        constexpr uint32_t Blue = 0xFF0000FF;
        constexpr uint32_t Red = 0xFFFF0000;
        constexpr uint32_t Gray = 0xFF202020;
        constexpr uint32_t Black = 0xFF000000;
        constexpr uint32_t White = 0xFFFFFFFF;
        
        uint32_t blendColors(uint32_t color1, uint32_t color2, float t);
    }
    
    struct GradientStop {
        uint32_t color;
        float position;  // 0-1
    };
    
    class LinearGradient {
    public:
        LinearGradient(GradientStop* stops, int stopCount, bool vertical = false);
        uint32_t colorAt(float position) const;
        
        bool isVertical() const { return vertical_; }
        
    private:
        GradientStop* stops_;
        int stopCount_;
        bool vertical_;
    };
}