#include "render.hpp"
#include "SDL2/SDL_render.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

const int RENDER_POSITION_CENTERED = -1;
const SDL_Color COLOR_WHITE = (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 };
const SDL_Color COLOR_BLACK = (SDL_Color) { .r = 0, .g = 0, .b = 0, .a = 0 };
const SDL_Color COLOR_YELLOW = (SDL_Color) { .r = 255, .g = 255, .b = 0, .a = 255 };

// Resources
TTF_Font* font;
Image images[IMAGE_COUNT];

// Resource management functions

bool render_load_resources() {
    font = TTF_OpenFont("./res/hack.ttf", 10);
    if(!font) {
        std::cout << "Unable to open font!" << TTF_GetError() << std::endl;
        return false;
    }

    render_load_spritesheet(IMAGE_PLAYER, "./res/witch.png", (vec2) { .x = 16, .y = 16 });
    render_load_image(IMAGE_MAP, "./res/maps/test.png");

    return true;
}

void render_free_resources() {
    TTF_CloseFont(font);

    for(int i = 0; i < IMAGE_COUNT; i++) {
        SDL_DestroyTexture(images[i].texture);
    }
}

void render_load_image(ImageName image_name, const char* path) {
    SDL_Surface* loaded_surface = IMG_Load(path);
    if(loaded_surface == nullptr) {
        std::cout << "Unable to load image " << path << "! SDL Error " << IMG_GetError() << std::endl;
        return;
    }

    images[image_name].texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);
    if(images[image_name].texture == nullptr) {
        std::cout << "Unable to create image texture! SDL Error " << SDL_GetError() << std::endl;
        return;
    }
    images[image_name].size = (vec2) {  .x = loaded_surface->w, .y = loaded_surface->h };
}

void render_load_spritesheet(ImageName image_name, const char* path, vec2 frame_size) {
    render_load_image(image_name, path);
    images[image_name].frame_size = frame_size;
}

// Rendering functions

void render_clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void render_present() {
    SDL_RenderPresent(renderer);
}

Image* render_create_text_image(const char* text, SDL_Color color) {
    SDL_Surface* text_surface = TTF_RenderText_Solid(font, text, color);
    if(text_surface == nullptr) {
        std::cout << "Unable to render text to surface! SDL Error " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if(text_texture == nullptr) {
        std::cout << "Unable to create text texture! SDL Error " << SDL_GetError() << std::endl;
        return nullptr;
    }


    Image* text_image = new (Image) {
        .texture = text_texture,
        .size = (vec2){ .x = text_surface->w, .y = text_surface->h },
        .frame_size = (vec2) { .x = 0, .y = 0 }
    };

    SDL_FreeSurface(text_surface);

    return text_image;
}

void render_text(const char* text, SDL_Color color, vec2 position) {
    Image* text_image = render_create_text_image(text, color);

    SDL_Rect source_rect = (SDL_Rect){ .x = 0, .y = 0, .w = text_image->size.x, .h = text_image->size.y };
    SDL_Rect dest_rect = (SDL_Rect){ .x = position.x, .y = position.y, .w = text_image->size.x, .h = text_image->size.y };
    if(dest_rect.x == RENDER_POSITION_CENTERED) {
        dest_rect.x = (SCREEN_WIDTH / 2) - (source_rect.w / 2);
    }
    if(dest_rect.y == RENDER_POSITION_CENTERED) {
        dest_rect.y = (SCREEN_HEIGHT / 2) - (source_rect.h / 2);
    }

    SDL_RenderCopy(renderer, text_image->texture, &source_rect, &dest_rect);
    SDL_DestroyTexture(text_image->texture);
    delete text_image;
}

void render_image(ImageName image_name, vec2 position) {
    SDL_Rect dst_rect = (SDL_Rect) {
        .x = position.x,
        .y = position.y,
        .w = images[image_name].size.x,
        .h = images[image_name].size.y,
    };

    SDL_Rect screen_rect = (SDL_Rect) { .x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT };
    if(!rects_intersect(dst_rect, screen_rect)) {
        return;
    }

    SDL_RenderCopy(renderer, images[image_name].texture, NULL, &dst_rect);
}

void render_image_frame(ImageName image_name, vec2 frame, vec2 position, bool flipped) {
    Image& image = images[image_name];

    SDL_Rect src_rect = (SDL_Rect) {
        .x = frame.x * image.frame_size.x,
        .y = frame.y * image.frame_size.y,
        .w = image.frame_size.x,
        .h = image.frame_size.y,
    };
    SDL_Rect dst_rect = (SDL_Rect) {
        .x = position.x,
        .y = position.y,
        .w = image.frame_size.x,
        .h = image.frame_size.y
    };

    if(src_rect.x < 0 || src_rect.x > image.size.x - image.frame_size.x
        || src_rect.y < 0 || src_rect.y > image.size.y - image.frame_size.y) {
        std::cout << "Index (" << frame.x << ", " << frame.y << ") out of bounds for image " << image_name << std::endl;
        return;
    }

    SDL_Rect screen_rect = (SDL_Rect) { .x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT };
    if(!rects_intersect(dst_rect, screen_rect)) {
        return;
    }

    SDL_RendererFlip render_flip = SDL_FLIP_NONE;
    if(flipped) {
        render_flip = SDL_FLIP_HORIZONTAL;
    }

    SDL_RenderCopyEx(renderer, image.texture, &src_rect, &dst_rect, 0, NULL, render_flip);
}
