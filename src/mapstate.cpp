#include "mapstate.hpp"
#include "vector.hpp"
#include "render.hpp"

void MapState::handle_input(SDL_Event e) {
    if(e.type == SDL_KEYDOWN) {
        SDL_Keycode keycode = e.key.keysym.sym;
        if(keycode == SDLK_ESCAPE) {
            finished = true;
        }
    }
}

void MapState::update() {

}

void MapState::render() {
}
