#pragma once

#include "widget.hpp"
#include <string>
#include <functional>
#include <memory>
#include "../core/signal.hpp"

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
        std::function<void()> onClick; // deprecated
    };
    
    class Button : public Widget {
    public:
        explicit Button(const ButtonConfig& config);
        
        void render() override;
        bool handleInput(const InputState& input) override;

        Signal<> onClick;       
        Signal<bool> onHover;   
        Signal<bool> onPress;   
        
    private:
        ButtonConfig config_;
        bool isHovered_ = false;
        bool isPressed_ = false;
    };
    
    // Factory function for easier creation
    std::shared_ptr<Button> ButtonWidget(const ButtonConfig& config);
}