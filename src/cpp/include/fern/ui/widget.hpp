#pragma once

#include "../core/types.hpp"
#include <functional>

namespace Fern {
    class Widget {
    public:
        virtual ~Widget() = default;
        virtual void render() = 0;
        virtual bool handleInput(const InputState& input) = 0;
    };
}