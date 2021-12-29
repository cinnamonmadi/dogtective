#pragma once

#include <SDL2/SDL.h>

typedef struct vec2 {
    int x;
    int y;
} vec2;

bool rects_intersect(const SDL_Rect& a, const SDL_Rect& b);
