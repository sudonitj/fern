#include <fern/fern.hpp>
#include <iostream>
#include <memory>

using namespace Fern;

static uint32_t pixels[600 * 800];
static int clickCount = 0;

void setupUI() {
    ButtonConfig config = {
        .x = 300, .y = 250, .width = 200, .height = 50,
        .normalColor = Colors::Green,
        .hoverColor = Colors::Green,
        .pressColor = Colors::DarkGreen,
        .label = "CLICK ME",
        .textScale = 2,
        .textColor = Colors::White
    };
    
    auto button = ButtonWidget(config);
    button->onClick.connect([]() {
        clickCount++;
        std::cout << "Clicked! Count: " << clickCount << std::endl;
    });
}

void draw() {
    Draw::fill(Colors::DarkGray);
    TextWidget(Point(50, 50), "BUTTON DEMO", 3, Colors::White);
    std::string counterText = "COUNT: " + std::to_string(clickCount);
    TextWidget(Point(50, 400), counterText.c_str(), 2, Colors::White);
}

int main() {
    Fern::initialize(pixels, 800, 600);
    setupUI();
    Fern::setDrawCallback(draw);
    Fern::startRenderLoop();
    return 0;
}