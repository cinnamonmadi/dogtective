#include "pause.hpp"

#include "render.hpp"

Pause::Pause() {
    render_previous = true;
}

void Pause::handle_input(SDL_Event e) {
    if(e.type == SDL_KEYDOWN) {
        SDL_Keycode keycode = e.key.keysym.sym;

        switch(keycode) {
            case SDLK_p:
                finished = true;
                break;
        }
    }
}

void Pause::update(float delta) {

}

void Pause::render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect box = (SDL_Rect) { .x = 10, .y = 10, .w = 100, .h = 100 };
    SDL_RenderFillRect(renderer, &box);
}
