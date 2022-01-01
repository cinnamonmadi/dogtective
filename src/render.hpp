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
typedef enum ImageName {
    IMAGE_PLAYER,
    IMAGE_MAP,
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
void render_load_image(ImageName image_name, const char* path);
void render_load_spritesheet(ImageName image_name, const char* path, vec2 frame_size);

// Render functions
void render_clear();
void render_present();
Image* render_create_text_image(const char* text, SDL_Color color);
void render_text(const char* text, SDL_Color color, vec2 position);
void render_image(ImageName image_name, vec2 position);
void render_image_frame(ImageName image_name, vec2 frame, vec2 position, bool flipped);
