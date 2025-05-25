#pragma once

// Include all component headers
#include "core/canvas.hpp"
#include "core/input.hpp"
#include "graphics/primitives.hpp"
#include "graphics/colors.hpp"
#include "text/font.hpp"
#include "ui/widgets.hpp"

namespace Fern {
    void initialize(uint32_t* pixelBuffer, int width, int height);
    void startRenderLoop();
    void setDrawCallback(std::function<void()> callback);
}