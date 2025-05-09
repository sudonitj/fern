#include "fern.c"

#define WIDTH 1200
#define HEIGHT 600

static uint32_t pixels[HEIGHT*WIDTH];

int main() {
    FernCanvas canvas = {pixels, HEIGHT, WIDTH};
    runApp(canvas);
    
    Container(
        color(0xFFFFD700),
        x(0),
        y(0),
        width(WIDTH),
        height(HEIGHT)
    );
    
    CircleWidget(
        radius(80),
        position(Point_create(WIDTH-200, 120)),
        color(0xFF87CEEB) 
    );
    
    Container(
        color(0xFF228B22),
        x(0),
        y(HEIGHT-150),
        width(WIDTH),
        height(150)
    );
    
    for (int i = 0; i < 180; i++) {
        int baseX = 100 + i * 400/180;  
        
        LineWidget(
            start(Point_create(baseX, HEIGHT-150)),  
            end(Point_create(300, HEIGHT-350)), 
            thickness(1),
            color(0xFF808080)  
        );
    }
    
    for (int i = 0; i < 180; i++) {
        int baseX = 700 + i * 400/180;  
        LineWidget(
            start(Point_create(baseX, HEIGHT-150)),
            end(Point_create(900, HEIGHT-400)),
            thickness(1),
            color(0xFF696969) 
        );
    }
    
    for (int i = 0; i < 50; i++) {
        int baseX = 850 + i * 100/50; 
        
        LineWidget(
            start(Point_create(baseX, HEIGHT-350)), 
            end(Point_create(900, HEIGHT-400)),
            thickness(1),
            color(0xFFFFFFFF) 
        );
    }   

    
    CircleWidget(
        radius(30),
        position(Point_create(200, 100)),
        color(0xFFFFFFFF)  
    );
    
    CircleWidget(
        radius(40),
        position(Point_create(240, 90)),
        color(0xFFFFFFFF) 
    );
    
    CircleWidget(
        radius(30),
        position(Point_create(280, 100)),
        color(0xFFFFFFFF)
    );
    
    CircleWidget(
        radius(25),
        position(Point_create(600, 150)),
        color(0xFFFFFFFF) 
    );
    
    CircleWidget(
        radius(35),
        position(Point_create(635, 140)),
        color(0xFFFFFFFF) 
    );
    
    CircleWidget(
        radius(25),
        position(Point_create(670, 150)),
        color(0xFFFFFFFF) 
    );
    
    fern_start_render_loop();
    return 0;
}