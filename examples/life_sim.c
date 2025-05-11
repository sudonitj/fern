// Particle Life Simulator - An artificial life simulation using Fern Graphics
#include "fern.c"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define WIDTH 800
#define HEIGHT 600
#define MAX_PARTICLES 2000
#define PARTICLE_TYPES 5
#define WORLD_MARGIN 50
#define INTERACTION_RADIUS 80.0f

static uint32_t pixels[HEIGHT*WIDTH];

// Basic 2D vector operations for simulation
typedef struct {
    float x;
    float y;
} Vector2D;

Vector2D vector_add(Vector2D a, Vector2D b) {
    return (Vector2D){a.x + b.x, a.y + b.y};
}

Vector2D vector_sub(Vector2D a, Vector2D b) {
    return (Vector2D){a.x - b.x, a.y - b.y};
}

Vector2D vector_scale(Vector2D v, float scale) {
    return (Vector2D){v.x * scale, v.y * scale};
}

float vector_length(Vector2D v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

Vector2D vector_normalize(Vector2D v) {
    float len = vector_length(v);
    if (len < 0.0001f) return (Vector2D){0, 0};
    return (Vector2D){v.x / len, v.y / len};
}

// Particle structure
typedef struct {
    Vector2D position;
    Vector2D velocity;
    int type;
    float size;
    bool active;
} Particle;

// Application state
typedef struct {
    Particle particles[MAX_PARTICLES];
    int particle_count;
    float attraction_matrix[PARTICLE_TYPES][PARTICLE_TYPES];
    uint32_t type_colors[PARTICLE_TYPES];
    float world_friction;
    float force_strength;
    float min_distance;
    float max_speed;
    bool paused;
    bool show_help;
    bool show_matrix;
    bool wrap_edges;
    int brush_type;
    float brush_size;
    int frame_count;
} AppState;

static AppState state = {0};

void set_brush_type(int type);
void apply_preset(int preset);

void set_brush_type_0() { set_brush_type(0); }
void set_brush_type_1() { set_brush_type(1); }
void set_brush_type_2() { set_brush_type(2); }
void set_brush_type_3() { set_brush_type(3); }
void set_brush_type_4() { set_brush_type(4); }

void apply_preset_0() { apply_preset(0); }
void apply_preset_1() { apply_preset(1); }
void apply_preset_2() { apply_preset(2); }
void apply_preset_3() { apply_preset(3); }
void apply_preset_4() { apply_preset(4); }

// Initialize the simulation
void init_simulation() {
    srand(time(NULL));
    
    // Set default parameters
    state.particle_count = 800;
    state.world_friction = 0.1f;
    state.force_strength = 0.5f;
    state.min_distance = 5.0f;
    state.max_speed = 3.0f;
    state.paused = false;
    state.show_help = true;
    state.show_matrix = false;
    state.wrap_edges = true;
    state.brush_type = 0;
    state.brush_size = 10.0f;
    
    // Set particle colors for each type
    state.type_colors[0] = 0xFFE74C3C;  // Red
    state.type_colors[1] = 0xFF3498DB;  // Blue
    state.type_colors[2] = 0xFF2ECC71;  // Green
    state.type_colors[3] = 0xFFF1C40F;  // Yellow
    state.type_colors[4] = 0xFF9B59B6;  // Purple
    
    // Randomize attraction/repulsion matrix between different types
    for (int i = 0; i < PARTICLE_TYPES; i++) {
        for (int j = 0; j < PARTICLE_TYPES; j++) {
            // Random value between -1 (repulsion) and 1 (attraction)
        state.attraction_matrix[i][j] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;        }
    }
    
    // Initialize particles with random positions and types
    for (int i = 0; i < state.particle_count; i++) {
        state.particles[i].position.x = WORLD_MARGIN + (float)rand() / RAND_MAX * (WIDTH - 2 * WORLD_MARGIN);
        state.particles[i].position.y = WORLD_MARGIN + (float)rand() / RAND_MAX * (HEIGHT - 2 * WORLD_MARGIN);
        state.particles[i].velocity.x = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        state.particles[i].velocity.y = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        state.particles[i].type = rand() % PARTICLE_TYPES;
        state.particles[i].size = 3.0f + ((float)rand() / RAND_MAX) * 2.0f;
        state.particles[i].active = true;
    }
}

// Add new particles at a position with the current brush type
void add_particles_at(float x, float y, int count) {
    for (int i = 0; i < count; i++) {
        if (state.particle_count >= MAX_PARTICLES) break;

        // This creates a circular spray pattern rather than putting all particles at exactly the same spot.
        
        float angle = ((float)rand() / RAND_MAX) * M_PI * 2.0f;
        float distance = ((float)rand() / RAND_MAX) * state.brush_size;
        
        state.particles[state.particle_count].position.x = x + cosf(angle) * distance;
        state.particles[state.particle_count].position.y = y + sinf(angle) * distance;
        state.particles[state.particle_count].velocity.x = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        state.particles[state.particle_count].velocity.y = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        state.particles[state.particle_count].type = state.brush_type;
        state.particles[state.particle_count].size = 3.0f + ((float)rand() / RAND_MAX) * 2.0f;
        state.particles[state.particle_count].active = true;
        
        state.particle_count++;
    }
}

// Reset the attraction matrix with new random values
void randomize_matrix() {
    for (int i = 0; i < PARTICLE_TYPES; i++) {
        for (int j = 0; j < PARTICLE_TYPES; j++) {
            state.attraction_matrix[i][j] = ((float)rand() / RAND_MAX) * 2.0f - 1.0f;
        }
    }
}

// This function sets up the relationship rules between particle types based on which preset (0-4) you choose.
void set_preset_matrix(int preset) {
    // Reset matrix
    for (int i = 0; i < PARTICLE_TYPES; i++) {
        for (int j = 0; j < PARTICLE_TYPES; j++) {
            state.attraction_matrix[i][j] = 0.0f;
        }
    }
    
    switch (preset) {
        case 0: // Segregation
            for (int i = 0; i < PARTICLE_TYPES; i++) {
                for (int j = 0; j < PARTICLE_TYPES; j++) {
                    if (i == j) state.attraction_matrix[i][j] = 0.9f;
                    else state.attraction_matrix[i][j] = -0.5f;
                }
            }
            break;
            
        case 1: // Chaos
            for (int i = 0; i < PARTICLE_TYPES; i++) {
                for (int j = 0; j < PARTICLE_TYPES; j++) {
                    state.attraction_matrix[i][j] = ((float)rand() / RAND_MAX) * 1.8f - 0.9f;
                }
            }
            break;
            
        case 2: // Symbiosis
            for (int i = 0; i < PARTICLE_TYPES; i++) {
                int friend = (i + 1) % PARTICLE_TYPES;
                for (int j = 0; j < PARTICLE_TYPES; j++) {
                    if (j == friend) state.attraction_matrix[i][j] = 0.8f;
                    else if (j == i) state.attraction_matrix[i][j] = 0.2f;
                    else state.attraction_matrix[i][j] = -0.3f;
                }
            }
            break;
            
        case 3: // Predator-Prey
            for (int i = 0; i < PARTICLE_TYPES; i++) {
                int prey = (i + 1) % PARTICLE_TYPES;
                int predator = (i + PARTICLE_TYPES - 1) % PARTICLE_TYPES;
                
                for (int j = 0; j < PARTICLE_TYPES; j++) {
                    if (j == prey) state.attraction_matrix[i][j] = 0.8f; // Chase prey
                    else if (j == predator) state.attraction_matrix[i][j] = -0.7f; // Run from predator
                    else if (j == i) state.attraction_matrix[i][j] = 0.2f; // Like own kind
                    else state.attraction_matrix[i][j] = 0.0f;
                }
            }
            break;
            
        case 4: // Harmony
            for (int i = 0; i < PARTICLE_TYPES; i++) {
                for (int j = 0; j < PARTICLE_TYPES; j++) {
                    state.attraction_matrix[i][j] = 0.3f;
                }
            }
            break;
    }
}

// Update particle positions and velocities based on interactions
void update_simulation() {
    if (state.paused) return;
    
    // For each active particle
    for (int i = 0; i < state.particle_count; i++) {
        if (!state.particles[i].active) continue;
        
        // Apply forces from nearby particles
        Vector2D total_force = {0.0f, 0.0f};
        
        for (int j = 0; j < state.particle_count; j++) {
            if (i == j || !state.particles[j].active) continue;
            
            Vector2D direction = vector_sub(state.particles[j].position, state.particles[i].position);
            
            // Handle wrapping for distance calculation
            if (state.wrap_edges) {
                if (direction.x > WIDTH / 2) direction.x -= WIDTH;
                if (direction.x < -WIDTH / 2) direction.x += WIDTH;
                if (direction.y > HEIGHT / 2) direction.y -= HEIGHT;
                if (direction.y < -HEIGHT / 2) direction.y += HEIGHT;
            }
            
            float distance = vector_length(direction);
            
            if (distance < INTERACTION_RADIUS && distance > 0.1f) {
                // Calculate force strength based on distance and type relationship
                float normalized_dist = distance / INTERACTION_RADIUS;
                int type_i = state.particles[i].type;
                int type_j = state.particles[j].type;
                
                float attraction = state.attraction_matrix[type_i][type_j];
                float force = 0.0f;
                
                if (distance < state.min_distance) {
                    // Strong repulsion when too close, regardless of attraction rules
                    force = -1.0f;
                } else {
                    // Force formula: Creates a "sweet spot" where particles prefer to be
                    float sweet_spot = INTERACTION_RADIUS * 0.5f;
                    if (distance < sweet_spot)
                        force = attraction * (distance / sweet_spot - 1.0f);
                    else
                        force = attraction * (1.0f - (distance - sweet_spot) / (INTERACTION_RADIUS - sweet_spot));
                }
                
                // Apply force
                Vector2D force_vector = vector_normalize(direction);
                force_vector = vector_scale(force_vector, force * state.force_strength);
                total_force = vector_add(total_force, force_vector);
            }
        }
        
        // Update velocity based on forces
        state.particles[i].velocity = vector_add(state.particles[i].velocity, total_force);
        
        // Apply friction
        state.particles[i].velocity = vector_scale(state.particles[i].velocity, 1.0f - state.world_friction);
        
        // Limit maximum speed
        float speed = vector_length(state.particles[i].velocity);
        if (speed > state.max_speed) {
            state.particles[i].velocity = vector_scale(
                vector_normalize(state.particles[i].velocity), state.max_speed
            );
        }
        
        // Update position
        state.particles[i].position = vector_add(state.particles[i].position, state.particles[i].velocity);
        
        // Handle boundary conditions
        if (state.wrap_edges) {
            // Wrap around edges
            if (state.particles[i].position.x < 0) 
                state.particles[i].position.x += WIDTH;
            if (state.particles[i].position.x >= WIDTH) 
                state.particles[i].position.x -= WIDTH;
            if (state.particles[i].position.y < 0) 
                state.particles[i].position.y += HEIGHT;
            if (state.particles[i].position.y >= HEIGHT) 
                state.particles[i].position.y -= HEIGHT;
        } else {
            // Bounce from walls
            if (state.particles[i].position.x < WORLD_MARGIN) {
                state.particles[i].position.x = WORLD_MARGIN;
                state.particles[i].velocity.x *= -0.8f;
            } else if (state.particles[i].position.x >= WIDTH - WORLD_MARGIN) {
                state.particles[i].position.x = WIDTH - WORLD_MARGIN - 1;
                state.particles[i].velocity.x *= -0.8f;
            }
            
            if (state.particles[i].position.y < WORLD_MARGIN) {
                state.particles[i].position.y = WORLD_MARGIN;
                state.particles[i].velocity.y *= -0.8f;
            } else if (state.particles[i].position.y >= HEIGHT - WORLD_MARGIN) {
                state.particles[i].position.y = HEIGHT - WORLD_MARGIN - 1;
                state.particles[i].velocity.y *= -0.8f;
            }
        }
    }
}

// Button callbacks
void toggle_help() {
    state.show_help = !state.show_help;
}

void toggle_pause() {
    state.paused = !state.paused;
}

void clear_particles() {
    state.particle_count = 0;
}

void toggle_wrap() {
    state.wrap_edges = !state.wrap_edges;
}

void toggle_matrix_view() {
    state.show_matrix = !state.show_matrix;
}

void randomize_rules() {
    randomize_matrix();
}

void set_brush_type(int type) {
    state.brush_type = type;
}

void apply_preset(int preset) {
    set_preset_matrix(preset);
}

void increase_force() {
    state.force_strength += 0.1f;
    if (state.force_strength > 2.0f) state.force_strength = 2.0f;
}

void decrease_force() {
    state.force_strength -= 0.1f;
    if (state.force_strength < 0.1f) state.force_strength = 0.1f;
}

// Draw the relationship matrix
void draw_relationship_matrix() {
    int size = 30;
    int spacing = 5;
    int start_x = 50;
    int start_y = 50;
    
    // Draw background
    Container(
        color(0xCC000000),
        x(start_x - 10),
        y(start_y - 10),
        width(PARTICLE_TYPES * (size + spacing) + 10),
        height(PARTICLE_TYPES * (size + spacing) + 50)
    );
    
    // Draw title
    TextWidget(
        Point_create(start_x, start_y - 30),
        "RELATIONSHIP MATRIX",
        1,
        0xFFFFFFFF
    );
    
    // Draw the matrix
    for (int i = 0; i < PARTICLE_TYPES; i++) {
        for (int j = 0; j < PARTICLE_TYPES; j++) {
            float relation = state.attraction_matrix[i][j];
            
            // Calculate color based on attraction/repulsion
            uint32_t cell_color;
            if (relation > 0) {
                // Attraction: Green gradient
                uint8_t intensity = (uint8_t)(relation * 255);
                cell_color = 0xFF000000 | (intensity << 8);
            } else {
                // Repulsion: Red gradient
                uint8_t intensity = (uint8_t)(-relation * 255);
                cell_color = 0xFF000000 | (intensity << 16);
            }
            
            // Draw cell
            Container(
                color(cell_color),
                x(start_x + j * (size + spacing)),
                y(start_y + i * (size + spacing)),
                width(size),
                height(size)
            );
            
            // Draw small type indicator in corner
            Container(
                color(state.type_colors[i]),
                x(start_x + j * (size + spacing)),
                y(start_y + i * (size + spacing)),
                width(8),
                height(8)
            );
            
            Container(
                color(state.type_colors[j]),
                x(start_x + j * (size + spacing) + size - 8),
                y(start_y + i * (size + spacing)),
                width(8),
                height(8)
            );
            
            // Show value
            char value[8];
            sprintf(value, "%.1f", relation);
            TextWidget(
                Point_create(start_x + j * (size + spacing) + 5, start_y + i * (size + spacing) + 10),
                value,
                1,
                0xFFFFFFFF
            );
        }
    }
    
    // Draw type colors legend
    for (int i = 0; i < PARTICLE_TYPES; i++) {
        Container(
            color(state.type_colors[i]),
            x(start_x + i * (size + spacing)),
            y(start_y + PARTICLE_TYPES * (size + spacing) + 10),
            width(size),
            height(15)
        );
        
        char label[2] = {0};
        label[0] = '0' + i;
        TextWidget(
            Point_create(start_x + i * (size + spacing) + 10, start_y + PARTICLE_TYPES * (size + spacing) + 22),
            label,
            1,
            0xFFFFFFFF
        );
    }
}

// Main drawing function
void draw_frame() {
    // Clear the canvas
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        pixels[i] = 0xFF000000;  // Black background
    }
    
    // Draw boundary if not wrapping
    if (!state.wrap_edges) {
        // Draw boundary rectangle
        for (int x = WORLD_MARGIN; x < WIDTH - WORLD_MARGIN; x++) {
            pixels[WORLD_MARGIN * WIDTH + x] = 0xFF333333;
            pixels[(HEIGHT - WORLD_MARGIN - 1) * WIDTH + x] = 0xFF333333;
        }
        
        for (int y = WORLD_MARGIN; y < HEIGHT - WORLD_MARGIN; y++) {
            pixels[y * WIDTH + WORLD_MARGIN] = 0xFF333333;
            pixels[y * WIDTH + (WIDTH - WORLD_MARGIN - 1)] = 0xFF333333;
        }
    }
    
    // Handle mouse interaction
    if (current_input.mouse_down && !state.show_help && !state.show_matrix) {
        // Add particles when dragging
        if (current_input.mouse_x > 0 && current_input.mouse_x < WIDTH && 
            current_input.mouse_y > 0 && current_input.mouse_y < HEIGHT) {
            add_particles_at(current_input.mouse_x, current_input.mouse_y, 3);
        }
    }
    
    // Update particles
    update_simulation();
    
    // Draw all particles
    for (int i = 0; i < state.particle_count; i++) {
        if (!state.particles[i].active) continue;
        
        int px = (int)state.particles[i].position.x;
        int py = (int)state.particles[i].position.y;
        int size = (int)state.particles[i].size;
        uint32_t color = state.type_colors[state.particles[i].type];
        
        // Draw circle
        for (int y = -size; y <= size; y++) {
            for (int x = -size; x <= size; x++) {
                if (x*x + y*y <= size*size) {
                    int draw_x = px + x;
                    int draw_y = py + y;
                    
                    // Wrap drawing positions if wrapping is enabled
                    if (state.wrap_edges) {
                        if (draw_x < 0) draw_x += WIDTH;
                        if (draw_x >= WIDTH) draw_x -= WIDTH;
                        if (draw_y < 0) draw_y += HEIGHT;
                        if (draw_y >= HEIGHT) draw_y -= HEIGHT;
                    }
                    
                    if (draw_x >= 0 && draw_x < WIDTH && draw_y >= 0 && draw_y < HEIGHT) {
                        pixels[draw_y * WIDTH + draw_x] = color;
                    }
                }
            }
        }
    }
    
    // Draw UI panel at the top
    Container(
        color(0xCC000000),
        x(0), y(0),
        width(WIDTH), height(40)
    );
    
    // Status text
    char status[100];
    sprintf(status, "Particles: %d  Force: %.1f  %s  %s", 
            state.particle_count,
            state.force_strength,
            state.paused ? "PAUSED" : "RUNNING",
            state.wrap_edges ? "WRAP" : "BOUNCE");
            
    TextWidget(
        Point_create(10, 15),
        status,
        1,
        0xFFFFFFFF
    );
    
    // Current brush type indicator
    TextWidget(
        Point_create(WIDTH - 120, 15),
        "BRUSH:",
        1,
        0xFFFFFFFF
    );
    
    Container(
        color(state.type_colors[state.brush_type]),
        x(WIDTH - 70),
        y(10),
        width(20),
        height(20)
    );
    
    // Draw UI buttons at the bottom
    Container(
        color(0xCC000000),
        x(0), y(HEIGHT - 50),
        width(WIDTH), height(50)
    );
    
    // Control buttons
    int button_width = 80;
    int button_height = 30;
    int button_spacing = 10;
    int start_x = 10;
    
    // Row 1 buttons
    ButtonConfig pause_button = {
        .x = start_x,
        .y = HEIGHT - 40,
        .width = button_width,
        .height = button_height,
        .normal_color = state.paused ? 0xFF339933 : 0xFF333399,
        .hover_color = state.paused ? 0xFF44AA44 : 0xFF4444AA,
        .press_color = state.paused ? 0xFF228822 : 0xFF222288,
        .label = state.paused ? "PLAY" : "PAUSE",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = toggle_pause
    };
    
    ButtonConfig clear_button = {
        .x = start_x + button_width + button_spacing,
        .y = HEIGHT - 40,
        .width = button_width,
        .height = button_height,
        .normal_color = 0xFF993333,
        .hover_color = 0xFFAA4444,
        .press_color = 0xFF882222,
        .label = "CLEAR",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = clear_particles
    };
    
    ButtonConfig wrap_button = {
        .x = start_x + 2 * (button_width + button_spacing),
        .y = HEIGHT - 40,
        .width = button_width,
        .height = button_height,
        .normal_color = state.wrap_edges ? 0xFF999933 : 0xFF336699,
        .hover_color = state.wrap_edges ? 0xFFAAAA44 : 0xFF4477AA,
        .press_color = state.wrap_edges ? 0xFF888822 : 0xFF225588,
        .label = state.wrap_edges ? "BOUNCE" : "WRAP",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = toggle_wrap
    };
    
    ButtonConfig matrix_button = {
        .x = start_x + 3 * (button_width + button_spacing),
        .y = HEIGHT - 40,
        .width = button_width,
        .height = button_height,
        .normal_color = 0xFF336699,
        .hover_color = 0xFF4477AA,
        .press_color = 0xFF225588,
        .label = "MATRIX",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = toggle_matrix_view
    };
    
    ButtonConfig random_button = {
        .x = start_x + 4 * (button_width + button_spacing),
        .y = HEIGHT - 40,
        .width = button_width,
        .height = button_height,
        .normal_color = 0xFF993399,
        .hover_color = 0xFFAA44AA,
        .press_color = 0xFF882288,
        .label = "RANDOM",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = randomize_rules
    };
    
    // Force buttons
    ButtonConfig force_up = {
        .x = start_x + 5 * (button_width + button_spacing),
        .y = HEIGHT - 40,
        .width = button_width / 2 - 5,
        .height = button_height,
        .normal_color = 0xFF339966,
        .hover_color = 0xFF44AA77,
        .press_color = 0xFF228855,
        .label = "UP",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = increase_force
    };
    
    ButtonConfig force_down = {
        .x = start_x + 5 * (button_width + button_spacing) + button_width / 2,
        .y = HEIGHT - 40,
        .width = button_width / 2 - 5,
        .height = button_height,
        .normal_color = 0xFF993366,
        .hover_color = 0xFFAA4477,
        .press_color = 0xFF882255,
        .label = "DN",
        .text_scale = 1,
        .text_color = 0xFFFFFFFF,
        .on_click = decrease_force
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
    
    // Draw buttons
    ButtonWidget(pause_button);
    ButtonWidget(clear_button);
    ButtonWidget(wrap_button);
    ButtonWidget(matrix_button);
    ButtonWidget(random_button);
    ButtonWidget(force_up);
    ButtonWidget(force_down);
    ButtonWidget(help_button);
    
    // Brush type selector buttons
    for (int i = 0; i < PARTICLE_TYPES; i++) {
        ButtonConfig brush_button = {
            .x = 10 + i * 45,
            .y = HEIGHT - 70,
            .width = 35,
            .height = 15,
            .normal_color = state.type_colors[i],
            .hover_color = state.type_colors[i] | 0xFF111111,
            .press_color = state.type_colors[i] & 0xFFEEEEEE,
            .on_click = NULL
        };
        
        // Set the click handler based on the type
        switch (i) {
            case 0: brush_button.on_click = set_brush_type_0; break;
            case 1: brush_button.on_click = set_brush_type_1; break;
            case 2: brush_button.on_click = set_brush_type_2; break;
            case 3: brush_button.on_click = set_brush_type_3; break;
            case 4: brush_button.on_click = set_brush_type_4; break;
        }
        
        ButtonWidget(brush_button);
    }
    
    // Preset buttons
    const char* preset_labels[] = {"TRIBE", "CHAOS", "SYMB", "HUNT", "HARM"};
    for (int i = 0; i < 5; i++) {
        ButtonConfig preset_button = {
            .x = WIDTH - 300 + i * 55,
            .y = HEIGHT - 70,
            .width = 50,
            .height = 15,
            .normal_color = 0xFF555566,
            .hover_color = 0xFF666677,
            .press_color = 0xFF444455,
            .label = preset_labels[i],
            .text_scale = 1,
            .text_color = 0xFFFFFFFF,
            .on_click = NULL
        };
        
        // Set the click handler based on the preset
        switch (i) {
            case 0: preset_button.on_click = apply_preset_0; break;
            case 1: preset_button.on_click = apply_preset_1; break;
            case 2: preset_button.on_click = apply_preset_2; break;
            case 3: preset_button.on_click = apply_preset_3; break;
            case 4: preset_button.on_click = apply_preset_4; break;
        }
        
        ButtonWidget(preset_button);
    }
    
    // Draw relationship matrix if enabled
    if (state.show_matrix) {
        draw_relationship_matrix();
    }
    
    // Help overlay
    if (state.show_help) {
        Container(
            color(0xDD000000),
            x(WIDTH/4), y(HEIGHT/4),
            width(WIDTH/2), height(HEIGHT/2)
        );
        
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 30), "PARTICLE LIFE SIMULATOR", 2, 0xFFFFFFFF);
        
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 70), 
                  "This simulation demonstrates emergent behaviors from", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 90), 
                  "simple attraction and repulsion rules between particles.", 1, 0xFFFFFFFF);
                  
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 120), 
                  "CONTROLS:", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 140), 
                  "- CLICK & DRAG: Add particles of selected type", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 160), 
                  "- PAUSE/PLAY: Toggle simulation", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 180), 
                  "- CLEAR: Remove all particles", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 200), 
                  "- WRAP/BOUNCE: Toggle edge behavior", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 220), 
                  "- MATRIX: Show relationship matrix", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 240), 
                  "- RANDOM: Randomize all relationships", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 260), 
                  "- F+/F-: Adjust force strength", 1, 0xFFFFFFFF);
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + 280), 
                  "- TRIBE/CHAOS/etc: Apply behavior presets", 1, 0xFFFFFFFF);
        
        TextWidget(Point_create(WIDTH/4 + 20, HEIGHT/4 + HEIGHT/4 - 30), 
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
    
    // Initialize the simulation
    init_simulation();
    
    // Set drawing callback for continuous rendering
    fern_set_draw_callback(draw_frame);
    
    // Start the render loop
    fern_start_render_loop();
    return 0;
}