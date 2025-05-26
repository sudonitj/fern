#include <fern/fern.hpp>
#include <emscripten.h>

using namespace Fern;

// Globals
static uint32_t pixels[600 * 800];

void draw() {
    // Clear screen
    Draw::fill(Colors::Gray);
    
    // Draw some shapes
    CircleWidget(400, Fern::Point(300, 100), Colors::Blue);
    Draw::rect(100, 100, 200, 150, Colors::Red);
    
    // Draw UI elements
    ButtonConfig button = {
        .x = 300,
        .y = 400,
        .width = 200,
        .height = 50,
        .normalColor = Colors::Green,
        .hoverColor = 0xFF44AA44,
        .pressColor = 0xFF228822,
        .label = "CLICK ME",
        .textScale = 2,
        .textColor = Colors::White,
        .onClick = []() {
            // Example button action
            EM_ASM({
                console.log("Button clicked!");
            });
        }
    };
    
    ButtonWidget(button);
    
    // Draw text
    TextWidget(Point(50, 50), "FERN CPP EXAMPLE", 3, Colors::White);
}

int main() {
    // Initialize the library
    Fern::initialize(pixels, 800, 600);
    
    // Set draw callback
    Fern::setDrawCallback(draw);
    
    // Start render loop
    Fern::startRenderLoop();
    
    return 0;
}