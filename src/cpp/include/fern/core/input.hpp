#pragma once

#include "types.hpp"

namespace Fern {
    class Input {
    public:
        static InputState& getState();
        static void resetEvents();
        
        // Called from JS/platform code
        static void updateMousePosition(int x, int y);
        static void updateMouseButton(bool down);
        
    private:
        static InputState state_;
    };
}