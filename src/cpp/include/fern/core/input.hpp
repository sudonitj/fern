#pragma once

#include "types.hpp"

namespace Fern {
    class Input {
    public:
        static InputState& getState();
        static void resetEvents();
        
        static void updateMousePosition(int x, int y);
        static void updateMouseButton(bool down);
        
    private:
        static InputState state_;
    };
}