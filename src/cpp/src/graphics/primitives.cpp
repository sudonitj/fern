#include "../../include/fern/graphics/primitives.hpp"
#include <cmath>

namespace Fern {
    namespace Draw {
        void fill(uint32_t color) {
            if (!globalCanvas) return;
            for (int i = 0; i < globalCanvas->getWidth() * globalCanvas->getHeight(); ++i) {
                globalCanvas->getBuffer()[i] = color;
            }
        }
        
        void rect(int x, int y, int width, int height, uint32_t color) {
            if (!globalCanvas) return;
            
            for (int dx = 0; dx < height; ++dx) {
                for (int dy = 0; dy < width; ++dy) {
                    int px = x + dy;
                    int py = y + dx;
                    if (px >= 0 && px < globalCanvas->getWidth() && 
                        py >= 0 && py < globalCanvas->getHeight()) {
                        globalCanvas->getBuffer()[py * globalCanvas->getWidth() + px] = color;
                    }
                }
            }
        }
        
        void circle(int cx, int cy, int radius, uint32_t color) {
            if (!globalCanvas) return;
            
            for (int x = -radius; x <= radius; x++) {
                for (int y = -radius; y <= radius; y++) {
                    if (x*x + y*y <= radius*radius) {
                        int px = cx + x;
                        int py = cy + y;
                        if (px >= 0 && px < globalCanvas->getWidth() && 
                            py >= 0 && py < globalCanvas->getHeight()) {
                            globalCanvas->getBuffer()[py * globalCanvas->getWidth() + px] = color;
                        }
                    }
                }
            }
        }
        
        void line(int x1, int y1, int x2, int y2, int thickness, uint32_t color) {
            if (!globalCanvas) return;
            
            int dx = std::abs(x2 - x1);
            int dy = std::abs(y2 - y1);
            int sx = x1 < x2 ? 1 : -1;
            int sy = y1 < y2 ? 1 : -1;
            int err = dx - dy;
            
            while (true) {
                circle(x1, y1, thickness, color);
                
                if (x1 == x2 && y1 == y2) break;
                
                int e2 = 2 * err;
                if (e2 > -dy) {
                    err -= dy;
                    x1 += sx;
                }
                if (e2 < dx) {
                    err += dx;
                    y1 += sy;
                }
            }
        }
    }
}