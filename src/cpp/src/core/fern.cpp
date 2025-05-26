#include "../../include/fern/fern.hpp"
#include "../../include/fern/core/input.hpp"
#include "../../include/fern/core/widget_manager.hpp"
#include <emscripten.h>
#include <functional>

namespace Fern {
    static std::function<void()> drawCallback = nullptr;
    
    void initialize(uint32_t* pixelBuffer, int width, int height) {
        globalCanvas = new Canvas(pixelBuffer, width, height);
        
        EM_ASM({
            var canvas = document.getElementById('canvas');
            
            canvas.addEventListener('mousemove', function(e) {
                var rect = canvas.getBoundingClientRect();
                var mouseX = Math.floor((e.clientX - rect.left) * (canvas.width / rect.width));
                var mouseY = Math.floor((e.clientY - rect.top) * (canvas.height / rect.height));
                
                Module._fernUpdateMousePosition(mouseX, mouseY);
            });
            
            canvas.addEventListener('mousedown', function(e) {
                Module._fernUpdateMouseButton(1);
            });
            
            canvas.addEventListener('mouseup', function(e) {
                Module._fernUpdateMouseButton(0);
            });
            
            console.log("Fern C++: Event listeners initialized");
        });
    }
    
    void startRenderLoop() {
        emscripten_set_main_loop([]() {
            if (drawCallback) {
                drawCallback();
            }

            WidgetManager::getInstance().updateAll(Input::getState());
            WidgetManager::getInstance().renderAll();
            
            EM_ASM({
                var canvas = document.getElementById('canvas');
                var ctx = canvas.getContext('2d');
                
                if (canvas.width !== $1 || canvas.height !== $0) {
                    canvas.width = $1;
                    canvas.height = $0;
                }
                
                var imageData = ctx.createImageData(canvas.width, canvas.height);
                var data = imageData.data;
                var buffer = $2;
                var size = $0 * $1;
                
                for (var i = 0; i < size; i++) {
                    var pixel = HEAP32[buffer/4 + i];
                    var r = pixel & 0xFF;
                    var g = (pixel >> 8) & 0xFF;
                    var b = (pixel >> 16) & 0xFF;
                    var a = (pixel >> 24) & 0xFF;
                    var j = i * 4;
                    data[j + 0] = r;
                    data[j + 1] = g;
                    data[j + 2] = b;
                    data[j + 3] = a;
                }
                
                ctx.putImageData(imageData, 0, 0);
            }, 
            globalCanvas->getHeight(), globalCanvas->getWidth(), 
            globalCanvas->getBuffer());
            
            Input::resetEvents();
        }, 0, 1);
    }
    
    void setDrawCallback(std::function<void()> callback) {
        drawCallback = callback;
    }
}

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void fernUpdateMousePosition(int x, int y) {
        Fern::Input::updateMousePosition(x, y);
    }
    
    EMSCRIPTEN_KEEPALIVE
    void fernUpdateMouseButton(int down) {
        Fern::Input::updateMouseButton(down != 0);
    }
}