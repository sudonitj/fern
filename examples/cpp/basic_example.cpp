#include <fern/fern.hpp>
#include <emscripten.h>
#include <iostream>
#include <memory>

using namespace Fern;

// Globals
static uint32_t pixels[600 * 800];
static std::shared_ptr<Button> myButton; 
static int clickCount = 0; 

void handleButtonClick() {
    clickCount++;
    std::cout << "Button clicked! Count: " << clickCount << std::endl;
}

void handleButtonHover(bool isHovered) {
    std::cout << (isHovered ? "Mouse entered button" : "Mouse left button") << std::endl;
}

void setupUI() {
    ButtonConfig config = {
        .x = 300,
        .y = 400,
        .width = 200,
        .height = 50,
        .normalColor = Colors::Green,
        .hoverColor = 0xFF44AA44,
        .pressColor = 0xFF228822,
        .label = "CLICK ME",
        .textScale = 2,
        .textColor = Colors::White
    };
    
    myButton = ButtonWidget(config);
    
    myButton->onClick.connect(handleButtonClick);
    myButton->onHover.connect(handleButtonHover);
}


void draw() {
    Draw::fill(Colors::Gray);
    
    CircleWidget(400, Fern::Point(300, 100), Colors::Blue);
    Draw::rect(100, 100, 200, 150, Colors::Red);
    
    TextWidget(Point(50, 50), "FERN CPP EXAMPLE", 3, Colors::White);
    
    std::string counterText = "CLICKS: " + std::to_string(clickCount);
    TextWidget(Point(50, 550), counterText.c_str(), 2, Colors::White);
    

    if (myButton) {
        myButton->handleInput(Input::getState());
        myButton->render();
    }
}

int main() {
    Fern::initialize(pixels, 800, 600);
    
    setupUI();
    
    Fern::setDrawCallback(draw);
    Fern::startRenderLoop();
    
    return 0;
}