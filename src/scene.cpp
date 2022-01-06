#include "scene.hpp"

#include "render.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>

using nlohmann::json;

Scene::Scene(std::string path) {
    std::ifstream map_file;
    map_file.open(path);
    if(!map_file.is_open()) {
        std::cout << "Unable to open scene file " << path << "!" << std::endl;
        return;
    }
    json map_json = json::parse(map_file);
    map_file.close();

    background_image = render_load_image(map_json["background"].get<std::string>());

    // Load colliders
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

    // Load actors
    json actor_jsons = map_json["actors"];
    for(json actor_json : actor_jsons) {
        std::string name = actor_json["name"].get<std::string>();
        std::string image_path = actor_json["image"].get<std::string>();

        Actor new_actor = Actor(name, image_path);
        new_actor.position = (vec2) {
            .x = actor_json["position"][0].get<int>(),
            .y = actor_json["position"][1].get<int>()
        };

        actors.push_back(new_actor);
    }

    // Create player
    actors.push_back(Actor("player", "./res/witch.png"));
    actor_player = get_actor_from_name("player");

    // Load scripts
    json script_jsons = map_json["scripts"];
    for(json script_json : script_jsons) {
        scripts.push_back(Script(script_json.get<std::string>()));
    }

    // Finalize map init
    for(int i = 0; i < 4; i++) {
        direction_key_pressed[i] = false;
    }
    player_direction = (vec2) { .x = 0, .y = 0 };

    camera_offset = (vec2) { .x = 0, .y = 0 };
}

void Scene::handle_input(SDL_Event e) {
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

void Scene::update(float delta) {
    actors[actor_player].velocity = player_direction;

    for(int i = 0; i < scripts.size(); i++) {
        while(script_execute(i, delta));
    }

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

        if(actors[i].position.x == actors[i].target.x && actors[i].position.y == actors[i].target.y) {
            actors[i].target = (vec2) { .x = -1, .y = -1 };
            actors[i].velocity = (vec2) { .x = 0, .y = 0 };
        }
    }
}

int Scene::get_actor_from_name(std::string name) {
    for(int i = 0; i < actors.size(); i++) {
        if(name == actors[i].name) {
            return i;
        }
    }

    std::cout << "Actor with name " << name << " does not exist in scene!" << std::endl;
    return -1;
}

bool Scene::script_execute(int script_index, float delta) {
    Script& script = scripts.at(script_index);
    if(script.is_finished()) {
        return false;
    }
    ScriptLine& line = script.current_line();

    switch(line.type) {
        case SCRIPT_MOVE:
            actors[get_actor_from_name(line.move.actor)].target = line.move.target;
            break;
        case SCRIPT_WAITFOR:
            if(actors[get_actor_from_name(line.waitfor.actor)].target.x != -1) {
                return false;
            }
            break;
        case SCRIPT_TURN:
            actors[get_actor_from_name(line.turn.actor)].facing_direction = line.turn.direction;
            break;
        case SCRIPT_DELAY:
            line.delay.timer -= delta;
            if(line.delay.timer > 0) {
                return false;
            }
            line.delay.timer = line.delay.duration;
            break;
        case SCRIPT_LOOP:
            script.restart();
            return true;
        default:
            break;
    }

    script.increment();

    return true;
}

void Scene::render() {
    render_image(background_image, camera_offset.inverse());
    for(Actor actor : actors) {
        actor.render(camera_offset);
    }
}
