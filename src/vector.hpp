#pragma once

#include <SDL2/SDL.h>

typedef struct vec2 {
    int x;
    int y;
    inline vec2 operator+(const vec2& other) const {
        return (vec2) { .x = this->x + other.x, .y = this->y + other.y };
    }
    inline vec2& operator+=(const vec2& other) {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }
    inline vec2 operator-(const vec2& other) const {
        return this->inverse() + other;
    }
    inline vec2& operator-=(const vec2& other) {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }
    inline vec2 inverse() const {
        return (vec2) { .x = this->x * -1, .y = this->y * -1 };
    }
} vec2;

bool rects_intersect(const SDL_Rect& a, const SDL_Rect& b);
bool vec2_in_rect(const vec2& v, const SDL_Rect& r);
