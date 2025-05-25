#pragma once

// Include all widget headers
#include "button.hpp"
#include "container.hpp"

// Additional widget function declarations would go here
namespace Fern {
    void CircleWidget(int radius, Point position, uint32_t color);
    void LineWidget(Point start, Point end, int thickness, uint32_t color);
    void TextWidget(Point start, const char* text, int scale, uint32_t color);
}