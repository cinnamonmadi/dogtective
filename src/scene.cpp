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

        new_actor.dialog = actor_json["dialog"].get<std::string>();

        if(actor_json.contains("path")) {
            for(json path_json : actor_json["path"]) {
                new_actor.path.push_back((Actor::PathNode) {
                    .position = (vec2) {
                        .x = path_json["position"][0].get<int>(),
                        .y = path_json["position"][1].get<int>()
                    },
                    .direction = get_direction_from_name(path_json["direction"].get<std::string>()),
                    .wait_duration = path_json["wait_duration"].get<float>()
                });
            }
        }

        actors.push_back(new_actor);
    }

    // Create player
    actors.push_back(Actor("player", "./res/witch.png"));
    // actor_player = get_actor_from_name("player");
    actor_player = actors.size() - 1;

    // Load scripts
    /*
    json script_jsons = map_json["scripts"];
    for(json script_json : script_jsons) {
        scripts.push_back(script_json.get<std::string>());
    }
    */

    // Finalize map init
    for(int i = 0; i < 4; i++) {
        direction_key_pressed[i] = false;
    }
    player_direction = (vec2) { .x = 0, .y = 0 };

    camera_offset = (vec2) { .x = 0, .y = 0 };
}

/*
Scene::Script::Script(std::string path) {
    // Used to turn the parsed text into one of the four direction indexes
    static std::string direction_names[4] = { "up", "right", "down", "left" };

    // Load the script file
    std::ifstream script_file;
    script_file.open(path);
    if(!script_file.is_open()) {
        std::cout << "Unable to open script file " << path << std::endl;
        return;
    }

    // Read the file line by line
    std::string script_line;
    while(getline(script_file, script_line)) {
        ScriptLine new_line;

        // Split the line by spaces
        std::string to_parse = script_line;
        std::vector<std::string> parts;
        while(to_parse.length() != 0) {
            std::size_t space_index = to_parse.find(" ");
            if(space_index == std::string::npos) {
                parts.push_back(to_parse);
                to_parse = "";
            } else {
                parts.push_back(to_parse.substr(0, space_index));
                to_parse = to_parse.substr(space_index + 1);
            }
        }

        // Parse the line
        if(parts[0] == "require") {
            required_actors.push_back(parts[1]);
            continue; // Continue to avoid pushing back a line for this command
        }else if(parts[0] == "move") {
            new_line.type = SCRIPT_MOVE;
            new_line.move.actor = parts[1];
            new_line.move.target = (vec2) {
                .x = std::stoi(parts[2]),
                .y = std::stoi(parts[3])
            };
        } else if(parts[0] == "waitfor") {
            new_line.type = SCRIPT_WAITFOR;
            new_line.waitfor.actor = parts[1];
        } else if(parts[0] == "turn") {
            new_line.type = SCRIPT_TURN;
            new_line.turn.actor = parts[1];
            for(int i = 0; i < 4; i++) {
                if(parts[2] == direction_names[i]) {
                    new_line.turn.direction = (Direction)i;
                }
            }
        } else if(parts[0] == "delay") {
            new_line.type = SCRIPT_DELAY;
            new_line.delay.duration = std::stof(parts[1]);
            new_line.delay.timer = new_line.delay.duration;
        }

        // Add the parsed line to the script
        lines.push_back(new_line);
    }

    // Add the new script to the scripts
    current_line = 0;
}
*/

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
            case SDLK_SPACE:
                player_interact();
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

void Scene::player_interact() {
    static const int SCANBOX_LENGTH = 4;
    SDL_Rect interact_scan_rect = actors[actor_player].get_rect();

    switch(actors[actor_player].facing_direction) {
        case DIRECTION_UP:
            interact_scan_rect.y -= SCANBOX_LENGTH;
            interact_scan_rect.h = SCANBOX_LENGTH;
            break;
        case DIRECTION_RIGHT:
            interact_scan_rect.x += interact_scan_rect.w;
            interact_scan_rect.w = SCANBOX_LENGTH;
            break;
        case DIRECTION_DOWN:
            interact_scan_rect.y += interact_scan_rect.h;
            interact_scan_rect.h = SCANBOX_LENGTH;
            break;
        case DIRECTION_LEFT:
            interact_scan_rect.x -= SCANBOX_LENGTH;
            interact_scan_rect.w = SCANBOX_LENGTH;
            break;
    }

    for(int i = 0; i < actors.size(); i++) {
        if(i == actor_player) {
            continue;
        }

        if(rects_intersect(interact_scan_rect, actors[i].get_rect())) {
            std::cout << actors[i].dialog << std::endl;
            return;
        }
    }
}

void Scene::update(float delta) {
    actors[actor_player].velocity = player_direction;

    /*
    for(int i = 0; i < scripts.size(); i++) {
        while(script_execute(i, delta));
    }
    */

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

/*
int Scene::get_actor_from_name(std::string name) {
    for(int i = 0; i < actors.size(); i++) {
        if(name == actors[i].name) {
            return i;
        }
    }

    std::cout << "Actor with name " << name << " does not exist in scene!" << std::endl;
    return -1;
}

void Scene::script_execute(int script_index, float delta) {
    Script& script = scripts.at(script_index);
    if(script.current_line == script.lines.size()) {
        return;
    }
    ScriptLine& line = script.lines[script.current_line];

    switch(line.type) {
        case SCRIPT_MOVE:
            actors[get_actor_from_name(line.move.actor)].target = line.move.target;
            break;
        case SCRIPT_WAITFOR:
            if(actors[get_actor_from_name(line.waitfor.actor)].target.x != -1) {
                return;
            }
            break;
        case SCRIPT_TURN:
            actors[get_actor_from_name(line.turn.actor)].facing_direction = line.turn.direction;
            break;
        case SCRIPT_DELAY:
            line.delay.timer -= delta;
            if(line.delay.timer > 0) {
                return;
            }
            line.delay.timer = line.delay.duration;
            break;
        default:
            break;
    }

    script.current_line++;

    script_execute(script_index, delta);
}
*/

void Scene::render() {
    render_image(background_image, camera_offset.inverse());
    for(Actor actor : actors) {
        actor.render(camera_offset);
    }
}
