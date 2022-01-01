#pragma once

#include <SDL2/SDL.h>

typedef struct ivec2 {
    int x;
    int y;
} ivec2;

typedef struct vec2 {
    float x;
    float y;
    inline ivec2 as_ivec2() const {
        return (ivec2) { .x = (int)round(x), .y = (int)round(y) };
    }
    inline vec2 operator+(const vec2& other) const {
        return (vec2) { .x = this->x + other.x, .y = this->y + other.y };
    }
    inline vec2& operator+=(const vec2& other) {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }
    inline vec2 operator-(const vec2& other) const {
        return *this + other.inverse();
    }
    inline vec2& operator-=(const vec2& other) {
        this->x -= other.x;
        this->y -= other.y;
        return *this;
    }
    inline vec2 operator*(const float scaler) const {
        return (vec2) { .x = this->x * scaler, .y = this->y * scaler };
    }
    inline vec2 inverse() const {
        return (vec2) { .x = this->x * -1, .y = this->y * -1 };
    }
    inline float length() const {
        return sqrt((x * x) + (y * y));
    }
    inline vec2 normalized() const {
        float length = this->length();
        if(length == 0) {
            return (vec2) { .x = 0, .y = 0 };
        } else {
            return (vec2) { .x = x / length, .y = y / length };
        }
    }
} vec2;

bool rects_intersect(const SDL_Rect& a, const SDL_Rect& b);
bool vec2_in_rect(const vec2& v, const SDL_Rect& r);
