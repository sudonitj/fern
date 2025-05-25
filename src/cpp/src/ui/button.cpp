#include "../../include/fern/ui/button.hpp"
#include "../../include/fern/core/input.hpp"
#include "../../include/fern/graphics/primitives.hpp"
#include "../../include/fern/text/font.hpp"
#include <cstring>

namespace Fern {
    Button::Button(const ButtonConfig& config)
        : config_(config) {}
    
    void Button::render() {
        uint32_t buttonColor = config_.normalColor;
        if (isHovered_) {
            buttonColor = isPressed_ ? config_.pressColor : config_.hoverColor;
        }
        
        Draw::rect(config_.x, config_.y, config_.width, config_.height, buttonColor);
        
        if (!config_.label.empty()) {
            int textWidth = config_.label.length() * 8 * config_.textScale;
            int textX = config_.x + (config_.width - textWidth) / 2;
            int textY = config_.y + (config_.height - 8 * config_.textScale) / 2;
            
            Text::drawText(config_.label.c_str(), textX, textY, config_.textScale, config_.textColor);
        }
    }
    
    bool Button::handleInput(const InputState& input) {
        isHovered_ = input.mouseX >= config_.x && 
                     input.mouseX < config_.x + config_.width &&
                     input.mouseY >= config_.y &&
                     input.mouseY < config_.y + config_.height;
        
        isPressed_ = isHovered_ && input.mouseDown;
        
        if (isHovered_ && input.mouseClicked && config_.onClick) {
            config_.onClick();
            return true;
        }
        
        return false;
    }
    
    void ButtonWidget(const ButtonConfig& config) {
        auto button = Button(config);
        const auto& input = Input::getState();
        button.handleInput(input);
        button.render();
    }
}