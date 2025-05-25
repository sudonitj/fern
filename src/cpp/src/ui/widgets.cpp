#include "../../include/fern/ui/widgets.hpp"
#include "../../include/fern/graphics/primitives.hpp"
#include "../../include/fern/text/font.hpp"

namespace Fern {
    void CircleWidget(int radius, Point position, uint32_t color) {
        Draw::circle(position.x, position.y, radius, color);
    }
    
    void LineWidget(Point start, Point end, int thickness, uint32_t color) {
        Draw::line(start.x, start.y, end.x, end.y, thickness, color);
    }
    
    void TextWidget(Point start, const char* text, int scale, uint32_t color) {
        Text::drawText(text, start.x, start.y, scale, color);
    }
}