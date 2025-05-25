#pragma once

#include <cstdint>

namespace Fern {
    namespace Text {
        void drawChar(char c, int x, int y, int scale, uint32_t color);
        void drawText(const char* text, int x, int y, int scale, uint32_t color);
    }
}