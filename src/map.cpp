#include "map.hpp"

#include "render.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>

using nlohmann::json;

Map::Map(std::string path) {
    std::ifstream map_file;
    map_file.open(path);
    if(!map_file.is_open()) {
        std::cout << "Unable to open map file " << path << "!" << std::endl;
        return;
    }
    json map_json = json::parse(map_file);
    map_file.close();

    background_image = render_load_image(map_json["background"].get<std::string>());
    json collider_arrays = map_json["colliders"];
    for(json collider_array : collider_arrays) {
        SDL_Rect collider = (SDL_Rect) {
            .x = collider_array[0].get<int>(),
            .y = collider_array[1].get<int>(),
            .w = collider_array[2].get<int>(),
            .h = collider_array[3].get<int>()
        };
        colliders.push_back(collider);
    }

    for(int i = 0; i < 4; i++) {
        direction_key_pressed[i] = false;
    }
    player_direction = (vec2) { .x = 0, .y = 0 };

    camera_offset = (vec2) { .x = 0, .y = 0 };
    actors.push_back(Actor("./res/witch.png"));
    actor_player = actors.size() - 1;

    actors.push_back(Actor("./res/witch.png"));
    actors[1].position = (vec2) { .x = 100, .y = 100 };
}

void Map::handle_input(SDL_Event e) {
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

void Map::update(float delta) {
    actors[actor_player].velocity = player_direction;

    for(int i = 0; i < actors.size(); i++) {
        actors[i].update(delta);

        SDL_Rect actor_rect = actors[i].get_rect();
        for(SDL_Rect collider : colliders) {
            if(rects_intersect(actor_rect, collider)) {
                actors[i].handle_collision(collider);
                break;
            }
        }

        for(int j = 0; j < actors.size(); j++) {
            if(i == j) {
                continue;
            }

            if(rects_intersect(actor_rect, actors[j].get_rect())) {
                actors[i].handle_collision(actors[j].get_rect());
                break;
            }
        }
    }
}

void Map::render() {
    render_image(background_image, camera_offset.inverse());
    for(Actor actor : actors) {
        actor.render(camera_offset);
    }
}

// Actor functions

