#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

// Game constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 360;
const int RENDER_POSITION_CENTERED = -1;

// Engine variables
SDL_Window* window;
SDL_Renderer* renderer;

bool engine_running = true;
bool engine_is_fullscreen = false;

// Timing variables
const float FRAME_DURATION = 1.0f / 60.0f;
float last_frame_time = 0.0f;
float last_second_time = 0.0f;
int frames_this_second = 0;
int fps = 0;

// Colors
const SDL_Color COLOR_WHITE = (SDL_Color) { .r = 255, .g = 255, .b = 255, .a = 255 };

// Resources
TTF_Font* font;
SDL_Texture* tileset;
int tileset_width;
int tileset_height;

// UI variables
const SDL_Rect UI_BORDER_MAIN = (SDL_Rect) { .x = 10, .y = 10, .w = 480, .h = 240 };
const SDL_Rect UI_BORDER_LOG = (SDL_Rect) { .x = 10, .y = 260, .w = 620, .h = 90 };
const SDL_Rect UI_BORDER_SIDEBAR = (SDL_Rect) { .x = 500, .y = 10, .w = 130, .h = 240 };

// Game loop functions
void input();
void update();
void render();

// Rendering helper functions
void render_clear();
void render_present();
void render_text(const char* text, SDL_Color color, int x, int y);
void render_tile(int tile_x, int tile_y, int pos_x, int pos_y);

// Engine functions
bool engine_init(int argc, char** argv);
void engine_quit();
void engine_set_resolution(int width, int height);
void engine_toggle_fullscreen();
void engine_clock_tick();

int main(int argc, char** argv) {
    if(!engine_init(argc, argv)) {
        return 0;
    }

    while(engine_running) {
        input();
        update();
        render();
        engine_clock_tick();
    }

    return 0;
}

// Game loop functions

void input() {
    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {
        if(e.type == SDL_QUIT) {
            engine_running = false;
        }
    }
}

void update() {}
void render() {
    render_clear();

    render_tile(5, 5, 0, 0);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &UI_BORDER_MAIN);
    SDL_RenderDrawRect(renderer, &UI_BORDER_LOG);
    SDL_RenderDrawRect(renderer, &UI_BORDER_SIDEBAR);

    // render_text(("FPS: " + std::to_string(fps)).c_str(), COLOR_WHITE, 0, 0);
    render_present();
}

// Render helper functions

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

void render_tile(int tile_x, int tile_y, int pos_x, int pos_y) {
    SDL_Rect src_rect = (SDL_Rect) { .x = 1 + (tile_x * 13), .y = 1 + (tile_y * 13), .w = 12, .h = 12 };
    SDL_Rect dst_rect = (SDL_Rect) { .x = UI_BORDER_MAIN.x + (pos_x * 12), .y = UI_BORDER_MAIN.y + (pos_y * 12), .w = 12, .h = 12 };

    if(src_rect.x < 0 || src_rect.x >= tileset_width || src_rect.y < 0 || src_rect.y >= tileset_height) {
        std::cout << "Error rendering tile! Tile (" << tile_x << ", " << tile_y << ") is out of bounds." << std::endl;
        return;
    }

    if(dst_rect.x < UI_BORDER_MAIN.x || dst_rect.x >= UI_BORDER_MAIN.x + UI_BORDER_MAIN.w || dst_rect.y < UI_BORDER_MAIN.y || dst_rect.y >= UI_BORDER_MAIN.y + UI_BORDER_MAIN.h) {
        return;
    }

    SDL_RenderCopy(renderer, tileset, &src_rect, &dst_rect);
}

// Engine functions

bool engine_init(int argc, char** argv) {
    bool init_fullscreened = false;
    int resolution_width = 1280;
    int resolution_height = 720;

    // Parse system arguments

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Unable to initialize SDL! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Plague Doctor", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    int img_flags = IMG_INIT_PNG;

    if(!(IMG_Init(img_flags) & img_flags)){
        std::cout << "Unable to initialize SDL_image! SDL Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if(TTF_Init() == -1){
        std::cout << "Unable to initialize SDL_ttf! SDL Error: " << TTF_GetError() << std::endl;
        return false;
    }

    if(!window || !renderer){
        std::cout << "Unable to initial engine!" << std::endl;
        return false;
    }

    font = TTF_OpenFont("./res/hack.ttf", 10);
    if(!font) {
        std::cout << "Unable to open font!" << TTF_GetError() << std::endl;
        return false;
    }

    SDL_Surface* tileset_surface = IMG_Load("./res/tileset.png");
    if(tileset_surface == nullptr) {
        std::cout << "Unable to load tileset image! SDL Error: " << IMG_GetError() << std::endl;
        return false;
    }

    tileset = SDL_CreateTextureFromSurface(renderer, tileset_surface);
    if(tileset == nullptr) {
        std::cout << "Unable to convert tileset surface into texture! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    tileset_width = tileset_surface->w;
    tileset_height = tileset_surface->h;
    SDL_FreeSurface(tileset_surface);

    engine_set_resolution(resolution_width, resolution_height);
    if(init_fullscreened) {
        engine_toggle_fullscreen();
    }

    return true;
}

void engine_quit() {
    SDL_DestroyTexture(tileset);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

void engine_set_resolution(int width, int height) {
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetWindowSize(window, width, height);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void engine_toggle_fullscreen() {
    if (engine_is_fullscreen){
        SDL_SetWindowFullscreen(window, 0);
    } else {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }
    engine_is_fullscreen = !engine_is_fullscreen;
}

void engine_clock_tick() {
    frames_this_second++;
    float current_time = SDL_GetTicks() / 1000.0f;

    // If one second has passed, record what the fps and dps was during that second
    if(current_time - last_second_time >= 1.0f) {
        fps = frames_this_second;
        frames_this_second = 0;
        last_second_time += 1.0;
    }

    // Delay if there's extra time between frames
    if(current_time - last_frame_time < FRAME_DURATION) {
        unsigned long delay_time = (unsigned long)(1000.0f * (FRAME_DURATION - (current_time - last_frame_time)));
        SDL_Delay(delay_time);
    }
}

