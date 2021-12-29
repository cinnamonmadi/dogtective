#pragma once

#include <SDL2/SDL.h>

// Externs defined in main.cpp
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern SDL_Renderer* renderer;

// Externs defined in render.cpp
extern const int RENDER_POSITION_CENTERED;
extern const SDL_Color COLOR_WHITE;

// Resource initialization
bool render_load_resources();
void render_free_resources();

// Render functions
void render_clear();
void render_present();
void render_text(const char* text, SDL_Color color, int x, int y);
