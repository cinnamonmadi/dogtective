#include "render.hpp"
#include "SDL2/SDL_render.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

const int RENDER_POSITION_CENTERED = -1;
const SDL_Color COLOR_WHITE = (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 };

// Resources
TTF_Font* font;
Image* images;
vec2* image_frame_sizes;

// Resource management functions

bool load_image(Image* image, const char* path);

bool render_load_resources() {
    font = TTF_OpenFont("./res/gba.ttf", 8);
    if(!font) {
        std::cout << "Unable to open font!" << TTF_GetError() << std::endl;
        return false;
    }

    image_frame_sizes = new vec2[IMAGE_COUNT];
    std::string image_paths[IMAGE_COUNT];

    image_paths[IMAGE_TILESET] = "tileset.png";
    image_frame_sizes[IMAGE_TILESET] = (vec2){ .x = 16, .y = 16 };

    images = new Image[IMAGE_COUNT];
    for(int i = 0; i < IMAGE_COUNT; i++) {
        if(!load_image(&images[i], ("./res/" + image_paths[i]).c_str())){
            return false;
        }
    }

    return true;
}

bool load_image(Image* image, const char* path) {
    SDL_Surface* loaded_surface = IMG_Load(path);
    if(loaded_surface == nullptr) {
        std::cout << "Unable to load image " << path << "! SDL Error " << IMG_GetError() << std::endl;
        return false;
    }

    image->texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    if(image->texture == nullptr) {
        std::cout << "Unable to create image texture! SDL Error " << SDL_GetError() << std::endl;
        return false;
    }
    image->width = loaded_surface->w;
    image->height = loaded_surface->h;

    return true;
}

void render_free_resources() {
    TTF_CloseFont(font);

    for(int i = 0; i < IMAGE_COUNT; i++) {
        SDL_DestroyTexture(images[i].texture);
    }
    delete [] images;
    delete [] image_frame_sizes;
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
    if(text_surface == nullptr) {
        std::cout << "Unable to render text to surface! SDL Error " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if(text_texture == nullptr) {
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

void render_image(Texture texture, vec2 frame, vec2 position) {
    SDL_Rect src_rect = (SDL_Rect) {
        .x = frame.x * image_frame_sizes[texture].x,
        .y = frame.y * image_frame_sizes[texture].y,
        .w = image_frame_sizes[texture].x,
        .h = image_frame_sizes[texture].y
    };
    SDL_Rect dst_rect = (SDL_Rect) {
        .x = position.x,
        .y = position.y,
        .w = image_frame_sizes[texture].x,
        .h = image_frame_sizes[texture].y
    };

    if(src_rect.x < 0 || src_rect.x > images[texture].width - image_frame_sizes[texture].x
        || src_rect.y < 0 || src_rect.y > images[texture].height - image_frame_sizes[texture].y) {
        std::cout << "Index (" << frame.x << ", " << frame.y << ") out of bounds for image " << texture << std::endl;
        return;
    }

    SDL_Rect screen_rect = (SDL_Rect) { .x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT };
    if(!rects_intersect(dst_rect, screen_rect)) {
        return;
    }

    SDL_RenderCopy(renderer, images[texture].texture, &src_rect, &dst_rect);
}
