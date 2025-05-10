#include "fern.c"
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

static uint32_t pixels[HEIGHT*WIDTH];

#define NEON_GREEN   0xFF00FF00
#define NEON_PINK    0xFFFF00FF
#define NEON_BLUE    0xFF00FFFF
#define NEON_YELLOW  0xFFFFFF00
#define NEON_ORANGE  0xFFFF7700
#define CYBER_BLACK  0xFF0A0A0A
#define CYBER_GRAY   0xFF2A2A2A
#define CYBER_PURPLE 0xFF8800FF
#define CYBER_RED    0xFFFF0066

void draw_sunset_gradient() {
    GradientStop sunset_stops[] = {
        {0xFF330066, 0.0}, 
        {0xFFFF6600, 0.4}, 
        {0xFF000033, 0.7},
        {0xFF000000, 1.0} 
    };
    
    LinearGradient sunset_gradient = {
        sunset_stops,
        4,               // steps
        GRADIENT_VERTICAL  // direction macro
    };
    LinearGradientContainer(0, 0, WIDTH, HEIGHT, sunset_gradient);
}

void draw_retro_grid() {
    int horizon = HEIGHT * 0.7;
    
    for (int i = 0; i < 10; i++) {
        int y = horizon - 10*i*i;
        if (y < 0) continue;
        
        LineWidget(
            start(Point_create(0, y)),
            end(Point_create(WIDTH, y)),
            thickness(1),
            color(NEON_PINK)
        );
    }
    
    int vanishing_x = WIDTH / 2;
    for (int x = 0; x <= WIDTH; x += 50) {
        int delta_x = x - vanishing_x;
        int y_top = delta_x != 0 ? horizon - (150000 / abs(delta_x)) : 0;
        if (y_top < 0) y_top = 0;
        
        LineWidget(
            start(Point_create(x, horizon)),
            end(Point_create(x, y_top)),
            thickness(1),
            color(NEON_PINK)
        );
    }
}

void draw_cyber_circuits() {
    int path_y = HEIGHT * 0.4;
    
    LineWidget(
        start(Point_create(50, path_y)),
        end(Point_create(WIDTH-50, path_y)),
        thickness(2),
        color(NEON_GREEN)
    );
    
    for (int x = 100; x < WIDTH-100; x += 150) {
        CircleWidget(
            radius(8),
            position(Point_create(x, path_y)),
            color(NEON_YELLOW)
        );
        
        int path_height = 30 + (x % 3) * 20;
        
        LineWidget(
            start(Point_create(x, path_y)),
            end(Point_create(x, path_y + path_height)),
            thickness(2),
            color(NEON_GREEN)
        );
        
        CircleWidget(
            radius(5),
            position(Point_create(x, path_y + path_height)),
            color(NEON_ORANGE)
        );
    }
}

void draw_digital_rain() {
    GradientStop rain_stops[] = {
        {NEON_GREEN, 0.0}, 
        {CYBER_BLACK, 1.0}  
    };
    
    LinearGradient rain_gradient = {
        rain_stops,
        2,
        GRADIENT_VERTICAL
    };
    
    for (int x = 30; x < WIDTH; x += 40) {
        int length = 50 + (x % 5) * 30;
        int start_y = (x * 7) % 100;
        
        for (int i = 0; i < length; i += 12) {
            float position = (float)i / length;
            
            uint32_t color = gradient_color_at(rain_gradient, position);
            char c = 'A' + (i % 26);
            fchar(pixels, WIDTH, HEIGHT, c, x, start_y + i, 1, color);
        }
    }
}
int main() {
    FernCanvas canvas = {pixels, HEIGHT, WIDTH};
    runApp(canvas);
    
    draw_sunset_gradient();
    
    draw_retro_grid();
    
    draw_cyber_circuits();
    
    draw_digital_rain();
    
    CircleWidget(
        radius(80),
        position(Point_create(WIDTH/2, HEIGHT/4)),
        color(NEON_ORANGE)
    );
    
    Container(
        color(CYBER_PURPLE),
        x(WIDTH/2 - 200),
        y(HEIGHT/2 - 50),
        width(120),
        height(80)
    );
    
    Container(
        color(NEON_BLUE),
        x(WIDTH/2 + 100),
        y(HEIGHT/2 - 80),
        width(80),
        height(80)
    );
    
    TextWidget(
        Point_create(WIDTH/2 - 130, 50),
        "CYBERPUNK",
        4,
        NEON_YELLOW
    );
    
    TextWidget(
        Point_create(100, HEIGHT-100),
        "FERN GRAPHICS ENGINE",
        2,
        CYBER_RED
    );
    fern_start_render_loop();
    return 0;
}