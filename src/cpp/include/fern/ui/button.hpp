#pragma once

#include "widget.hpp"
#include <string>
#include <functional>

namespace Fern {
    struct ButtonConfig {
        int x;
        int y;
        int width;
        int height;
        uint32_t normalColor;
        uint32_t hoverColor;
        uint32_t pressColor;
        std::string label;
        int textScale;
        uint32_t textColor;
        std::function<void()> onClick;
    };
    
    class Button : public Widget {
    public:
        explicit Button(const ButtonConfig& config);
        
        void render() override;
        bool handleInput(const InputState& input) override;
        
    private:
        ButtonConfig config_;
        bool isHovered_ = false;
        bool isPressed_ = false;
    };
    
    // Factory function for easier creation
    void ButtonWidget(const ButtonConfig& config);
}