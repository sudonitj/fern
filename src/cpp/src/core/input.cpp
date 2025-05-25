#include "../../include/fern/core/input.hpp"

namespace Fern {
    InputState Input::state_ = {};
    
    InputState& Input::getState() {
        return state_;
    }
    
    void Input::resetEvents() {
        state_.mouseClicked = false;
    }
    
    void Input::updateMousePosition(int x, int y) {
        state_.mouseX = x;
        state_.mouseY = y;
    }
    
    void Input::updateMouseButton(bool down) {
        if (!state_.mouseDown && down) {
            state_.mouseClicked = true;
        }
        state_.mouseDown = down;
    }
}