#include "render.hpp"

#include <SDL2/SDL_ttf.h>
#include <iostream>

const int RENDER_POSITION_CENTERED = -1;

const SDL_Color COLOR_WHITE = (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 };

// Resources
TTF_Font* font;

// Resource management functions

bool render_load_resources() {
    font = TTF_OpenFont("./res/gba.ttf", 8);
    if(!font) {
        std::cout << "Unable to open font!" << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

void render_free_resources() {
    TTF_CloseFont(font);
}

// Rendering functions

void render_clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void render_present() {
    SDL_RenderPresent(renderer);
}

void render_text(const char* text, SDL_Color color, int x, int y) {
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, color);
    if(text_surface == NULL) {
        std::cout << "Unable to render text to surface! SDL Error " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if(text_texture == NULL) {
        std::cout << "Unable to create text texture! SDL Error " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect source_rect = (SDL_Rect){ .x = 0, .y = 0, .w = text_surface->w, .h = text_surface->h };
    SDL_Rect dest_rect = (SDL_Rect){ .x = x, .y = y, .w = text_surface->w, .h = text_surface->h };
    if(dest_rect.x == RENDER_POSITION_CENTERED) {
        dest_rect.x = (SCREEN_WIDTH / 2) - (source_rect.w / 2);
    }
    if(dest_rect.y == RENDER_POSITION_CENTERED) {
        dest_rect.y = (SCREEN_HEIGHT / 2) - (source_rect.h / 2);
    }

    SDL_RenderCopy(renderer, text_texture, &source_rect, &dest_rect);

    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}
