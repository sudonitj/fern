// Fractal Explorer - An interactive Mandelbrot set visualization using Fern Graphics
#include "fern.c"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_ITERATIONS 100
#define COLOR_SCHEMES 6

static uint32_t pixels[HEIGHT*WIDTH];

// Fractal state
typedef struct {
    double center_x;
    double center_y;
    double zoom;
    int max_iterations;
    int color_scheme;
    bool dragging;
    double drag_start_x;
    double drag_start_y;
    double start_center_x;
    double start_center_y;
    int frame_count;
    bool auto_iterate;
    bool show_help;
    bool high_quality;
} FractalState;

static FractalState state = {
    .center_x = -0.5,
    .center_y = 0.0,
    .zoom = 4.0,
    .max_iterations = 100,
    .color_scheme = 0,
    .dragging = false,
    .auto_iterate = false,
    .show_help = true,
    .high_quality = false,
    .frame_count = 0
};

// Color palettes
static uint32_t color_palettes[COLOR_SCHEMES][16] = {
    // Classic blue-gold
    {0xFF000033, 0xFF000066, 0xFF000099, 0xFF0000CC, 0xFF0000FF, 0xFF0033FF, 
     0xFF0066FF, 0xFF0099FF, 0xFF00CCFF, 0xFFCCBB99, 0xFFDDCC88, 0xFFEEDD77, 
     0xFFFFEE66, 0xFFFFFF55, 0xFFFFFFAA, 0xFFFFFFFF},
    
    // Fire
    {0xFF000000, 0xFF330000, 0xFF660000, 0xFF990000, 0xFFCC0000, 0xFFFF0000, 
     0xFFFF3300, 0xFFFF6600, 0xFFFF9900, 0xFFFFCC00, 0xFFFFFF00, 0xFFFFFFAA, 
     0xFFFFFFCC, 0xFFFFFFDD, 0xFFFFFFEE, 0xFFFFFFFF},
     
    // Electric
    {0xFF000033, 0xFF000066, 0xFF000099, 0xFF0000CC, 0xFF0033FF, 0xFF00CCFF, 
     0xFF00FFCC, 0xFF33FFAA, 0xFF66FF99, 0xFF99FF66, 0xFFCCFF33, 0xFFFFFF00, 
     0xFFFFAA00, 0xFFFF5500, 0xFFFF0000, 0xFFFFFFFF},
     
    // Grayscale
    {0xFF000000, 0xFF111111, 0xFF222222, 0xFF333333, 0xFF444444, 0xFF555555,
     0xFF666666, 0xFF777777, 0xFF888888, 0xFF999999, 0xFFAAAAAA, 0xFFBBBBBB,
     0xFFCCCCCC, 0xFFDDDDDD, 0xFFEEEEEE, 0xFFFFFFFF},
     
    // Aquatic
    {0xFF000022, 0xFF000044, 0xFF000066, 0xFF000088, 0xFF0000AA, 0xFF0022BB, 
     0xFF0044CC, 0xFF0066DD, 0xFF0088EE, 0xFF00AAFF, 0xFF22BBFF, 0xFF44CCFF, 
     0xFF66DDFF, 0xFF88EEFF, 0xFFAAFFFF, 0xFFFFFFFF},
     
    // Psychedelic
    {0xFFFF00FF, 0xFFFF33CC, 0xFFFF6699, 0xFFFF9966, 0xFFFFCC33, 0xFFFFFF00, 
     0xFFCCFF33, 0xFF99FF66, 0xFF66FF99, 0xFF33FFCC, 0xFF00FFFF, 0xFF33CCFF, 
     0xFF6699FF, 0xFF9966FF, 0xFFCC33FF, 0xFFFF00FF}
};

// Function to calculate mandelbrot value for a point
int calculate_mandelbrot(double cr, double ci, int max_iter) {
    double zr = 0;
    double zi = 0;
    double zr2 = 0;
    double zi2 = 0;
    int i;
    
    for (i = 0; i < max_iter; i++) {
        zi = 2 * zr * zi + ci;
        zr = zr2 - zi2 + cr;
        zr2 = zr * zr;
        zi2 = zi * zi;
        
        if (zr2 + zi2 > 4)
            break;
    }
    
    return i;
}

// Map screen coordinates to mathematical coordinates
void screen_to_math(int px, int py, double *x, double *y) {
    double aspect = (double)WIDTH / HEIGHT;
    *x = state.center_x + ((double)px / WIDTH - 0.5) * state.zoom * aspect;
    *y = state.center_y + ((double)py / HEIGHT - 0.5) * state.zoom;
}

// Get color for a point in the mandelbrot set
uint32_t get_mandelbrot_color(int iterations, int max_iterations) {
    if (iterations == max_iterations)
        return 0xFF000000;  // Black for points in the set
    
    // Select color palette
    uint32_t* palette = color_palettes[state.color_scheme];
    
    // Smooth coloring using logarithmic scale
    double smoothed = (double)iterations / max_iterations;
    smoothed = sqrt(smoothed) * 15;  // Use sqrt for more pleasing gradient
    int index = (int)smoothed;
    
    // Linear interpolation between colors
    uint32_t color1 = palette[index % 16];
    uint32_t color2 = palette[(index + 1) % 16];
    double t = smoothed - index;
    
    uint8_t r1 = (color1 >> 16) & 0xFF;
    uint8_t g1 = (color1 >> 8) & 0xFF;
    uint8_t b1 = color1 & 0xFF;
    
    uint8_t r2 = (color2 >> 16) & 0xFF;
    uint8_t g2 = (color2 >> 8) & 0xFF;
    uint8_t b2 = color2 & 0xFF;
    
    uint8_t r = r1 * (1-t) + r2 * t;
    uint8_t g = g1 * (1-t) + g2 * t;
    uint8_t b = b1 * (1-t) + b2 * t;
    
    return (0xFF << 24) | (r << 16) | (g << 8) | b;
}

// Render the fractal
void render_fractal() {
    int step = state.high_quality ? 1 : 2;  // Quality setting
    
    for (int y = 0; y < HEIGHT; y += step) {
        for (int x = 0; x < WIDTH; x += step) {
            double cr, ci;
            screen_to_math(x, y, &cr, &ci);
            
            int iterations = calculate_mandelbrot(cr, ci, state.max_iterations);
            uint32_t color = get_mandelbrot_color(iterations, state.max_iterations);
            
            for (int dy = 0; dy < step && y+dy < HEIGHT; dy++) {
                for (int dx = 0; dx < step && x+dx < WIDTH; dx++) {
                    pixels[(y+dy) * WIDTH + (x+dx)] = color;
                }
            }
        }
    }
}

// Button callbacks
void toggle_help() {
    state.show_help = !state.show_help;
}

void reset_view() {
    state.center_x = -0.5;
    state.center_y = 0.0;
    state.zoom = 4.0;
}

void toggle_auto_iterate() {
    state.auto_iterate = !state.auto_iterate;
}

void next_color_scheme() {
    state.color_scheme = (state.color_scheme + 1) % COLOR_SCHEMES;
}

void increase_iterations() {
    if (state.max_iterations < 500)
        state.max_iterations += 20;
}

void decrease_iterations() {
    if (state.max_iterations > 20)
        state.max_iterations -= 20;
}

void toggle_quality() {
    state.high_quality = !state.high_quality;
}

void go_to_interesting_location() {
    // Interesting locations in the Mandelbrot set
    static int location_index = 0;
    static struct {
        double x, y, zoom;
        const char* name;
    } locations[] = {
        {-0.5, 0.0, 4.0, "Full Set"},
        {-0.7436447860, 0.1318259043, 0.005, "Seahorse Valley"},
        {-0.7453, 0.1127, 0.005, "Elephant Valley"},
        {-0.16070135, 1.0375665, 0.02, "Triple Spiral Valley"},
        {-1.25066, 0.02012, 0.0005, "Mini Mandelbrot"},
        {0.281717921930775, 0.5771052841488, 0.0000001, "Spiral Decoration"},
    };
    
    location_index = (location_index + 1) % 6;
    state.center_x = locations[location_index].x;
    state.center_y = locations[location_index].y;
    state.zoom = locations[location_index].zoom;
    
    // Show name of location
    char buffer[50];
    sprintf(buffer, "Location: %s", locations[location_index].name);
    fernPrintf(buffer);
}

// Drawing function called every frame
void draw_frame() {
    // Update state
    if (state.auto_iterate && state.frame_count % 10 == 0) {
        state.max_iterations = 50 + 50 * sin(state.frame_count * 0.01);
        if (state.max_iterations < 20) state.max_iterations = 20;
    }
    
    // Handle mouse interactions
    if (current_input.mouse_down) {
        if (!state.dragging) {
            state.dragging = true;
            state.drag_start_x = current_input.mouse_x;
            state.drag_start_y = current_input.mouse_y;
            state.start_center_x = state.center_x;
            state.start_center_y = state.center_y;
        } else {
            // Pan the view
            double dx = (current_input.mouse_x - state.drag_start_x) / WIDTH * state.zoom;
            double dy = (current_input.mouse_y - state.drag_start_y) / HEIGHT * state.zoom;
            double aspect = (double)WIDTH / HEIGHT;
            state.center_x = state.start_center_x - dx * aspect;
            state.center_y = state.start_center_y - dy;
        }
    } else {
        state.dragging = false;
    }
    
    // Handle mouse wheel (simulated with keys here)
    if (current_input.mouse_clicked) {
        double cr, ci;
        screen_to_math(current_input.mouse_x, current_input.mouse_y, &cr, &ci);
        
        if (current_input.mouse_y < 50) {
            // Zoom out if clicking on top bar
            state.zoom *= 1.5;
        } else if (!state.dragging) {
            // Zoom in and center on click point
            state.center_x = cr;
            state.center_y = ci;
            state.zoom *= 0.5;
        }
    }
    
    // Render the fractal
    render_fractal();
    
    // Draw UI elements on top
    // Top info bar
    Container(
        color(0xAA000000),
        x(0), y(0),
        width(WIDTH), height(50)
    );
    
    // Show coordinates and zoom level
    char info[100];
    sprintf(info, "Center: %.8f, %.8f  Zoom: %.8f  Iterations: %d", 
            state.center_x, state.center_y, state.zoom, state.max_iterations);
    
    TextWidget(
        Point_create(10, 15),
        info,
        1,
        0xFFFFFFFF
    );
    
    // Bottom controls bar
    Container(
        color(0xAA000000),
        x(0), y(HEIGHT - 50),
        width(WIDTH), height(50)
    );
    
    // Control buttons
    int button_width = 90;
    int button_height = 30;
    int button_spacing = 10;
    int start_x = 10;
    
    ButtonConfig reset_button = {
        .x = start_x,
        .y = HEIGHT - 40,
        .width = button_width,
        .height = button_height,
        .normal_color = 0xFF333399,
        .hover_color = 0xFF4444AA,
        .press_color = 0xFF222288,
        .label = "RESET VIEW",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = reset_view
    };
    
    ButtonConfig color_button = {
        .x = start_x + button_width + button_spacing,
        .y = HEIGHT - 40,
        .width = button_width,
        .height = button_height,
        .normal_color = 0xFF339933,
        .hover_color = 0xFF44AA44,
        .press_color = 0xFF228822,
        .label = "NEXT COLOR",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = next_color_scheme
    };
    
    ButtonConfig iter_up_button = {
        .x = start_x + 2*(button_width + button_spacing),
        .y = HEIGHT - 40,
        .width = button_width/2,
        .height = button_height,
        .normal_color = 0xFF993333,
        .hover_color = 0xFFAA4444,
        .press_color = 0xFF882222,
        .label = "ITER+",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = increase_iterations
    };
    
    ButtonConfig iter_down_button = {
        .x = start_x + 2*(button_width + button_spacing) + button_width/2,
        .y = HEIGHT - 40,
        .width = button_width/2,
        .height = button_height,
        .normal_color = 0xFF993333,
        .hover_color = 0xFFAA4444,
        .press_color = 0xFF882222,
        .label = "ITER-",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = decrease_iterations
    };
    
    ButtonConfig auto_button = {
        .x = start_x + 3*(button_width + button_spacing),
        .y = HEIGHT - 40,
        .width = button_width,
        .height = button_height,
        .normal_color = state.auto_iterate ? 0xFF993399 : 0xFF333366,
        .hover_color = state.auto_iterate ? 0xFFAA44AA : 0xFF444477,
        .press_color = state.auto_iterate ? 0xFF882288 : 0xFF222255,
        .label = "AUTO ITER",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = toggle_auto_iterate
    };
    
    ButtonConfig quality_button = {
        .x = start_x + 4*(button_width + button_spacing),
        .y = HEIGHT - 40,
        .width = button_width,
        .height = button_height,
        .normal_color = state.high_quality ? 0xFF999933 : 0xFF333366,
        .hover_color = state.high_quality ? 0xFFAAAA44 : 0xFF444477,
        .press_color = state.high_quality ? 0xFF888822 : 0xFF222255,
        .label = state.high_quality ? "HIGH QUALITY" : "LOW QUALITY",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = toggle_quality
    };
    
    ButtonConfig tour_button = {
        .x = start_x + 5*(button_width + button_spacing),
        .y = HEIGHT - 40,
        .width = button_width,
        .height = button_height,
        .normal_color = 0xFF336699,
        .hover_color = 0xFF4477AA,
        .press_color = 0xFF225588,
        .label = "NEXT SPOT",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = go_to_interesting_location
    };
    
    ButtonConfig help_button = {
        .x = WIDTH - 60,
        .y = HEIGHT - 40,
        .width = 50,
        .height = button_height,
        .normal_color = 0xFF666666,
        .hover_color = 0xFF777777,
        .press_color = 0xFF555555,
        .label = "HELP",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = toggle_help
    };
    
    ButtonWidget(reset_button);
    ButtonWidget(color_button);
    ButtonWidget(iter_up_button);
    ButtonWidget(iter_down_button);
    ButtonWidget(auto_button);
    ButtonWidget(quality_button);
    ButtonWidget(tour_button);
    ButtonWidget(help_button);
    
    // Help overlay
    if (state.show_help) {
        Container(
            color(0xCC000000),
            x(WIDTH/4), y(HEIGHT/4),
            width(WIDTH/2), height(HEIGHT/2)
        );
        
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 30), "FRACTAL EXPLORER", 2, 0xFFFFFFFF);
        
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 70), 
                  "- DRAG: Pan around the fractal", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 90), 
                  "- CLICK: Zoom in at that point", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 110), 
                  "- CLICK TOP BAR: Zoom out", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 130), 
                  "- RESET: Return to default view", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 150), 
                  "- COLOR: Change color scheme", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 170), 
                  "- ITER+/-: Adjust iteration depth", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 190), 
                  "- AUTO ITER: Animate iteration depth", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 210), 
                  "- QUALITY: Toggle rendering quality", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 230), 
                  "- NEXT SPOT: Tour interesting locations", 1, 0xFFFFFFFF);
        
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 270), 
                  "CLICK ANYWHERE TO CLOSE HELP", 1, 0xFFFFFFFF);
        
        if (current_input.mouse_clicked) {
            state.show_help = false;
        }
    }
    
    state.frame_count++;
}

int main() {
    // Initialize the canvas
    FernCanvas canvas = {pixels, HEIGHT, WIDTH};
    runApp(canvas);
    
    // Set drawing callback for continuous rendering
    fern_set_draw_callback(draw_frame);
    
    // Start the render loop
    fern_start_render_loop();
    return 0;
}