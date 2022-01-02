#include "mapstate.hpp"

#include <iostream>

MapState::MapState() {
    map = Map();
    map.load("./map/test.json");
    player_direction = (vec2) { .x = 0, .y = 0 };
    for(int i = 0; i < 4; i++) {
        direction_key_pressed[i] = false;
    }
}

void MapState::handle_input(SDL_Event e) {
    if(e.type == SDL_KEYDOWN) {
        SDL_Keycode keycode = e.key.keysym.sym;

        switch(keycode) {
            case SDLK_ESCAPE:
                finished = true;
                break;
            case SDLK_w:
                player_direction.y = -1;
                direction_key_pressed[DIRECTION_UP] = true;
                break;
            case SDLK_s:
                player_direction.y = 1;
                direction_key_pressed[DIRECTION_DOWN] = true;
                break;
            case SDLK_a:
                player_direction.x = -1;
                direction_key_pressed[DIRECTION_LEFT] = true;
                break;
            case SDLK_d:
                player_direction.x = 1;
                direction_key_pressed[DIRECTION_RIGHT] = true;
                break;
        }
    } else if(e.type == SDL_KEYUP) {
        SDL_Keycode keycode = e.key.keysym.sym;

        switch(keycode) {
            case SDLK_w:
                if(direction_key_pressed[DIRECTION_DOWN]) {
                    player_direction.y = 1;
                } else {
                    player_direction.y = 0;
                }
                direction_key_pressed[DIRECTION_UP] = false;
                break;
            case SDLK_s:
                if(direction_key_pressed[DIRECTION_UP]) {
                    player_direction.y = -1;
                } else {
                    player_direction.y = 0;
                }
                direction_key_pressed[DIRECTION_DOWN] = false;
                break;
            case SDLK_a:
                if(direction_key_pressed[DIRECTION_RIGHT]) {
                    player_direction.x = 1;
                } else {
                    player_direction.x = 0;
                }
                direction_key_pressed[DIRECTION_LEFT] = false;
                break;
            case SDLK_d:
                if(direction_key_pressed[DIRECTION_LEFT]) {
                    player_direction.x = -1;
                } else {
                    player_direction.x = 0;
                }
                direction_key_pressed[DIRECTION_RIGHT] = false;
                break;
        }
    }
}

void MapState::update(float delta) {
    map.player.velocity = player_direction;
    map.update(delta);
}

void MapState::render() {
    map.render();
}
