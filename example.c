#include "fern.c"

#define WIDTH 800
#define HEIGHT 600

static uint32_t pixels[HEIGHT*WIDTH];

static int frame_count = 0;
static int circle_x = 400;
static int circle_y = 300;
static int dx = 2;  

void on_button_click() {
    fernPrintf("Button clicked!");
    circle_y += 50; 
    if (circle_y > HEIGHT - 50) {
        circle_y = 50; 
    }
}

void draw_frame() {
    Container(
        color(Colors_black),
        x(0),
        y(0),
        width(WIDTH),
        height(HEIGHT)
    );
    
    CircleWidget(
        radius(50),
        position(Point_create(circle_x, circle_y)),
        color(Colors_red)
    );
    
    circle_x += dx;
    if (circle_x > WIDTH - 50 || circle_x < 50) {
        dx = -dx;  
    }
    
    char text[50];
    sprintf(text, "FRAME: %d", frame_count++);
    
    TextWidget(
        Point_create(50, 50),
        text,
        2,
        Colors_white
    );
    
    char mouse_text[50];
    sprintf(mouse_text, "MOUSE: %d, %d", current_input.mouse_x, current_input.mouse_y);
    
    TextWidget(
        Point_create(50, 100),
        mouse_text,
        2,
        Colors_white
    );
    
    ButtonConfig test_button = {
        .x = 300,
        .y = 400,
        .width = 200,
        .height = 80,
        .normal_color = 0xFF0000FF,
        .hover_color = 0xFF4444FF,
        .press_color = 0xFF000088,
        .label = "CLICK ME",
        .text_scale = 2,
        .text_color = Colors_white,
        .on_click = on_button_click
    };
    
    ButtonWidget(test_button);
}

int main() {
    // Initialize the canvas
    FernCanvas canvas = {pixels, HEIGHT, WIDTH};
    runApp(canvas);
    
    // Set our drawing function as the callback
    fern_set_draw_callback(draw_frame);
    
    // Start the render loop
    fern_start_render_loop();
    return 0;
}