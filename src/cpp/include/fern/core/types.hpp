#pragma once

#include <cstdint>
#include <functional>

namespace Fern {
    struct Point {
        int x = 0;
        int y = 0;
        
        Point() = default;
        Point(int x, int y) : x(x), y(y) {}
    };
    
    struct InputState {
        int mouseX = 0;
        int mouseY = 0;
        bool mouseDown = false;
        bool mouseClicked = false;
    };
}