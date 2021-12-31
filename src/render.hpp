#pragma once

#include "vector.hpp"
#include <SDL2/SDL.h>

// Externs defined in main.cpp
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern SDL_Renderer* renderer;

// Externs defined in render.cpp
extern const int RENDER_POSITION_CENTERED;
extern const SDL_Color COLOR_WHITE;
extern const SDL_Color COLOR_BLACK;
extern const SDL_Color COLOR_YELLOW;

// Resources
typedef enum Texture {
    IMAGE_TILESET,
    IMAGE_COUNT
} Texture;

typedef struct Image {
    SDL_Texture* texture;
    vec2 size;
    vec2 frame_size;
} Image;

// Resource initialization
bool render_load_resources();
void render_free_resources();
int render_load_image(const char* path);
int render_load_spritesheet(const char* path, vec2 frame_size);

// Render functions
void render_clear();
void render_present();
int render_text(const char* text, SDL_Color color, int x, int y);
void render_image(int image_index, vec2 position);
void render_image_frame(int image_index, vec2 frame, vec2 position);
