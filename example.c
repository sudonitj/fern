#include "fern.c"

#define WIDTH 800
#define HEIGHT 600
#define MAX_STROKES 10000
#define MAX_COLORS 12

static uint32_t pixels[HEIGHT*WIDTH];

// Application state
typedef struct {
    int x;
    int y;
    int size;
    uint32_t color;
} DrawPoint;

static DrawPoint strokes[MAX_STROKES];
static int stroke_count = 0;
static int current_color = 0;
static int brush_size = 5;
static int prev_mouse_x = -1;
static int prev_mouse_y = -1;
static bool drawing = false;
static bool show_help = false;
static int frame_count = 0;

static uint32_t color_palette[MAX_COLORS] = {
    0xFF000000,  // Black
    0xFFFFFFFF,  // White
    0xFFFF0000,  // Red
    0xFF00FF00,  // Green
    0xFF0000FF,  // Blue
    0xFFFFFF00,  // Yellow
    0xFF00FFFF,  // Cyan
    0xFFFF00FF,  // Magenta
    0xFFFFA500,  // Orange
    0xFF800080,  // Purple
    0xFF006400,  // Dark Green
    0xFF8B4513   // Brown
};

void draw_line(int x0, int y0, int x1, int y1, int size, uint32_t color) {
    if (stroke_count >= MAX_STROKES - 10) return;
    
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    
    while (true) {
        strokes[stroke_count].x = x0;
        strokes[stroke_count].y = y0;
        strokes[stroke_count].size = size;
        strokes[stroke_count].color = color;
        stroke_count++;
        
        if (x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if (e2 >= dy) {
            if (x0 == x1) break;
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            if (y0 == y1) break;
            err += dx;
            y0 += sy;
        }
        
        if (stroke_count >= MAX_STROKES - 2) break;
    }
}

void clear_canvas() {
    stroke_count = 0;
    fernPrintf("Canvas cleared");
}

void increase_brush() {
    brush_size += 2;
    if (brush_size > 20) brush_size = 20;
    fernPrintf("Brush size increased");
}

void decrease_brush() {
    brush_size -= 2;
    if (brush_size < 1) brush_size = 1;
    fernPrintf("Brush size decreased");
}

void toggle_help() {
    show_help = !show_help;
}

void undo_last_stroke() {
    int points_to_remove = 100;
    if (stroke_count < points_to_remove) {
        stroke_count = 0;
    } else {
        stroke_count -= points_to_remove;
    }
    fernPrintf("Undo last stroke");
}

void select_color(int color_index) {
    if (color_index >= 0 && color_index < MAX_COLORS) {
        current_color = color_index;
        fernPrintf("Selected color");
    }
}

void select_color_0() { select_color(0); }
void select_color_1() { select_color(1); }
void select_color_2() { select_color(2); }
void select_color_3() { select_color(3); }
void select_color_4() { select_color(4); }
void select_color_5() { select_color(5); }
void select_color_6() { select_color(6); }
void select_color_7() { select_color(7); }
void select_color_8() { select_color(8); }
void select_color_9() { select_color(9); }

void draw_frame() {
    Container(
        color(0xFFEEEEEE), 
        x(0), 
        y(0),
        width(WIDTH),
        height(HEIGHT)
    );
    
    Container(
        color(0xFFFFFFFF),
        x(50),
        y(80),
        width(WIDTH - 100),
        height(HEIGHT - 150)
    );
    
    for (int i = 0; i < stroke_count; i++) {
        CircleWidget(
            radius(strokes[i].size),
            position(Point_create(strokes[i].x, strokes[i].y)),
            color(strokes[i].color)
        );
    }
    
    TextWidget(
        Point_create(50, 40),
        "COLORS",
        2,
        0xFF000000
    );
    
    for (int i = 0; i < MAX_COLORS; i++) {
        int box_size = 30;
        int spacing = 5;
        int x_pos = 215 + (i * (box_size + spacing));
        int y_pos = 30;
        
        if (i == current_color) {
            Container(
                color(0xFF000000),
                x(x_pos - 2),
                y(y_pos - 2),
                width(box_size + 4),
                height(box_size + 4)
            );
        }
        
        ButtonConfig color_button = {
            .x = x_pos,
            .y = y_pos,
            .width = box_size,
            .height = box_size,
            .normal_color = color_palette[i],
            .hover_color = color_palette[i],
            .press_color = color_palette[i],
            .label = "",
            .text_scale = 0,
            .text_color = 0,
            .on_click = NULL  
        };
        
        switch(i) {
            case 0: color_button.on_click = select_color_0; break;
            case 1: color_button.on_click = select_color_1; break;
            case 2: color_button.on_click = select_color_2; break;
            case 3: color_button.on_click = select_color_3; break;
            case 4: color_button.on_click = select_color_4; break;
            case 5: color_button.on_click = select_color_5; break;
            case 6: color_button.on_click = select_color_6; break;
            case 7: color_button.on_click = select_color_7; break;
            case 8: color_button.on_click = select_color_8; break;
            case 9: color_button.on_click = select_color_9; break;
        }
        
        ButtonWidget(color_button);
    }
    
    ButtonConfig clear_button = {
        .x = 50,
        .y = HEIGHT - 50,
        .width = 160,
        .height = 40,
        .normal_color = 0xFFFF5555,
        .hover_color = 0xFFFF7777,
        .press_color = 0xFFFF3333,
        .label = "CLEAR",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = clear_canvas
    };
    
    ButtonConfig undo_button = {
        .x = 230,
        .y = HEIGHT - 50,
        .width = 160,
        .height = 40,
        .normal_color = 0xFF5555FF,
        .hover_color = 0xFF7777FF,
        .press_color = 0xFF3333FF,
        .label = "UNDO",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = undo_last_stroke
    };
    
    ButtonConfig increase_button = {
        .x = 410,
        .y = HEIGHT - 50,
        .width = 80,
        .height = 40,
        .normal_color = 0xFF55AA55,
        .hover_color = 0xFF77CC77,
        .press_color = 0xFF338833,
        .label = "INCREASE",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = increase_brush
    };
    
    ButtonConfig decrease_button = {
        .x = 500,
        .y = HEIGHT - 50,
        .width = 80,
        .height = 40,
        .normal_color = 0xFF55AA55,
        .hover_color = 0xFF77CC77,
        .press_color = 0xFF338833,
        .label = "DECREASE",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = decrease_brush
    };
    
    ButtonConfig help_button = {
        .x = 600,
        .y = HEIGHT - 50,
        .width = 100,
        .height = 40,
        .normal_color = 0xFF555555,
        .hover_color = 0xFF777777,
        .press_color = 0xFF333333,
        .label = "HELP",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = toggle_help
    };
    
    ButtonWidget(clear_button);
    ButtonWidget(undo_button);
    ButtonWidget(increase_button);
    ButtonWidget(decrease_button);
    ButtonWidget(help_button);
    
    char size_text[20];
    sprintf(size_text, "SIZE: %d", brush_size);
    
    TextWidget(
        Point_create(WIDTH - 150, 40),
        size_text,
        2,
        0xFF000000
    );
    
    CircleWidget(
        radius(brush_size),
        position(Point_create(WIDTH - 80, 45)),
        color(color_palette[current_color])
    );
    
    if (show_help) {
        Container(
            color(0xCC000000),
            x(150),
            y(150),
            width(WIDTH - 300),
            height(HEIGHT - 300)
        );
        
        TextWidget(
            Point_create(200, 180),
            "DRAWING APP HELP",
            2,
            0xFFFFFFFF
        );
        
        TextWidget(
            Point_create(200, 230),
            "- CLICK AND DRAG TO DRAW",
            1,
            0xFFFFFFFF
        );
        
        TextWidget(
            Point_create(200, 260),
            "- SELECT COLORS FROM PALETTE",
            1,
            0xFFFFFFFF
        );
        
        TextWidget(
            Point_create(200, 290),
            "- ADJUST BRUSH SIZE WITH +/- BUTTONS",
            1,
            0xFFFFFFFF
        );
        
        TextWidget(
            Point_create(200, 320),
            "- CLICK UNDO TO REMOVE LAST STROKES",
            1,
            0xFFFFFFFF
        );
        
        TextWidget(
            Point_create(200, 350),
            "- CLICK CLEAR TO RESET CANVAS",
            1,
            0xFFFFFFFF
        );
        
        TextWidget(
            Point_create(200, 400),
            "CLICK ANYWHERE TO CLOSE THIS HELP",
            1,
            0xFFFFFFFF
        );
        
        if (current_input.mouse_clicked) {
            if (current_input.mouse_x < 150 || current_input.mouse_x > WIDTH - 150 ||
                current_input.mouse_y < 150 || current_input.mouse_y > HEIGHT - 150) {
                show_help = false;
                fernPrintf("Help closed by outside click");
            }
        }
    }
    
    if (current_input.mouse_x >= 50 && current_input.mouse_x <= WIDTH - 50 &&
        current_input.mouse_y >= 80 && current_input.mouse_y <= HEIGHT - 70) {
        
        if (current_input.mouse_down) {
            if (!drawing) {
                drawing = true;
                prev_mouse_x = current_input.mouse_x;
                prev_mouse_y = current_input.mouse_y;
            } else {
                draw_line(
                    prev_mouse_x, 
                    prev_mouse_y, 
                    current_input.mouse_x, 
                    current_input.mouse_y, 
                    brush_size, 
                    color_palette[current_color]
                );
                
                prev_mouse_x = current_input.mouse_x;
                prev_mouse_y = current_input.mouse_y;
            }
        } else {
            drawing = false;
        }
    }
    
    char debug_text[50];
    sprintf(debug_text, "STROKES: %d", stroke_count);
    
    TextWidget(
        Point_create(50, HEIGHT - 80),
        debug_text,
        1,
        0xFF333333
    );
    
    frame_count++;
}

int main() {
    FernCanvas canvas = {pixels, HEIGHT, WIDTH};
    runApp(canvas);    
    fern_set_draw_callback(draw_frame);    
    fern_start_render_loop();
    return 0;
}