#include "scene.hpp"

#include "render.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>

using nlohmann::json;

const float DIALOG_CHAR_SPEED = 0.05;

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

        for(json dialog_json : actor_json["dialog"]) {
            new_actor.dialog.push_back((DialogLine) {
                .speaker = dialog_json["speaker"].get<std::string>(),
                .text = dialog_json["text"].get<std::string>()
            });
        }

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
    actor_player = actors.size() - 1;

    // Load scripts
    json script_jsons = map_json["scripts"];
    for(json script_json : script_jsons) {
        Script new_script;

        for(json required_actor : script_json["requires"]) {
            new_script.required_actors.push_back(required_actor.get<std::string>());
        }
        for(json script_line_json : script_json["lines"]) {
            ScriptLine new_line;
            std::string line_type = script_line_json["type"].get<std::string>();

            // Parse the line
            if(line_type == "move") {
                new_line.type = SCRIPT_MOVE;
                new_line.move.actor = script_line_json["actor"].get<std::string>();
                new_line.move.target = (vec2) {
                    .x = script_line_json["position"][0].get<int>(),
                    .y = script_line_json["position"][1].get<int>(),
                };
            } else if(line_type == "waitfor") {
                new_line.type = SCRIPT_WAITFOR;
                new_line.waitfor.actor = script_line_json["actor"].get<std::string>();
            } else if(line_type == "turn") {
                new_line.type = SCRIPT_TURN;
                new_line.turn.actor = script_line_json["actor"].get<std::string>();
                new_line.turn.direction = get_direction_from_name(script_line_json["direction"].get<std::string>());
            } else if(line_type == "delay") {
                new_line.type = SCRIPT_DELAY;
                new_line.delay.duration = script_line_json["duration"].get<float>();
                new_line.delay.timer = new_line.delay.duration;
            } else if(line_type == "dialog") {
                new_line.type = SCRIPT_DIALOG;
                new_line.dialog.has_been_opened = false;
                for(json dialog_line_json : script_line_json["lines"]) {
                    new_line.dialog.lines.push_back((DialogLine) {
                        .speaker = dialog_line_json["speaker"].get<std::string>(),
                        .text = dialog_line_json["text"].get<std::string>()
                    });
                }
            }

            new_script.lines.push_back(new_line);
        }

        new_script.playing = false;
        new_script.current_line = 0;
        scripts.push_back(new_script);
    }

    // Finalize map init
    for(int i = 0; i < 4; i++) {
        direction_key_pressed[i] = false;
    }
    player_direction = (vec2) { .x = 0, .y = 0 };

    camera_offset = (vec2) { .x = 0, .y = 0 };

    dialog_open = false;
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
            case SDLK_SPACE:
                if(dialog_open) {
                    if(dialog_index != dialog_queue[0].text.length()) {
                        dialog_index = dialog_queue[0].text.length();
                    } else {
                        dialog_queue.erase(dialog_queue.begin());
                        if(dialog_queue.empty()) {
                            dialog_open = false;
                        } else {
                            dialog_index = 1;
                        }
                    }
                } else {
                    // TODO only do this while no script is playing
                    // Also update the scripts for loop because we really only want to run one script at a time
                    player_interact();
                }
                break;
            case SDLK_e:
                script_begin(0);
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


void Scene::open_dialog(const std::vector<DialogLine>& dialog_lines) {
    dialog_queue.clear();
    for(DialogLine dialog_line : dialog_lines) {
        dialog_queue.push_back(dialog_line);
    }
    dialog_index = 1;
    dialog_index_timer = DIALOG_CHAR_SPEED;
    dialog_open = true;
}

void Scene::update(float delta) {
    player_handle_input(delta);

    for(int i = 0; i < scripts.size(); i++) {
        if(scripts[i].playing) {
            script_execute(i, delta);
        }
    }

    for(int i = 0; i < actors.size(); i++) {
        actor_update(i, delta);
    }
}

void Scene::player_handle_input(float delta) {
    if(dialog_open) {
        actors[actor_player].velocity = (vec2) { .x = 0, .y = 0 };

        if(dialog_index != dialog_queue[0].text.length()) {
            dialog_index_timer -= delta;
            if(dialog_index_timer <= 0) {
                dialog_index++;
                dialog_index_timer = DIALOG_CHAR_SPEED;
            }
        }
    } else if (!actors[actor_player].in_scene) {
        actors[actor_player].velocity = player_direction;
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
            open_dialog(actors[i].dialog);
            return;
        }
    }
}

void Scene::actor_update(int actor_index, float delta) {
    Actor& actor = actors[actor_index];

    actor.update(delta);

    SDL_Rect actor_rect = actor.get_rect();
    for(SDL_Rect collider : colliders) {
        if(rects_intersect(actor_rect, collider)) {
            actor.handle_collision(collider);
            break;
        }
    }

    for(int j = 0; j < actors.size(); j++) {
        if(actor_index == j) {
            continue;
        }

        if(rects_intersect(actor_rect, actors[j].get_rect())) {
            actor.handle_collision(actors[j].get_rect());
            break;
        }
    }
}

// Scripts
//
int Scene::get_actor_from_name(std::string name) {
    for(int i = 0; i < actors.size(); i++) {
        if(name == actors[i].name) {
            return i;
        }
    }

    std::cout << "Actor with name " << name << " does not exist in scene!" << std::endl;
    return -1;
}

void Scene::script_begin(int script_index) {
    scripts[script_index].current_line = 0;
    scripts[script_index].playing = true;

    for(std::string required_actor : scripts[script_index].required_actors) {
        actors[get_actor_from_name(required_actor)].in_scene = true;
    }
}

void Scene::script_finish(int script_index) {
    scripts[script_index].playing = false;

    for(std::string required_actor : scripts[script_index].required_actors) {
        actors[get_actor_from_name(required_actor)].in_scene = false;
    }
}

void Scene::script_execute(int script_index, float delta) {
    Script& script = scripts.at(script_index);
    if(script.current_line == script.lines.size()) {
        script_finish(script_index);
        return;
    }
    ScriptLine& line = script.lines[script.current_line];

    switch(line.type) {
        case SCRIPT_MOVE:
            actors[get_actor_from_name(line.move.actor)].target = line.move.target;
            break;
        case SCRIPT_WAITFOR:
            if(actors[get_actor_from_name(line.waitfor.actor)].has_target()) {
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
        case SCRIPT_DIALOG:
            if(!line.dialog.has_been_opened) {
                open_dialog(line.dialog.lines);
                line.dialog.has_been_opened = true;
                return;
            } else if(dialog_open) {
                return;
            }
            break;
        default:
            break;
    }

    script.current_line++;

    script_execute(script_index, delta);
}

void Scene::render() {
    render_image(background_image, camera_offset.inverse());
    for(Actor actor : actors) {
        actor.render(camera_offset);
    }
    if(dialog_open) {
        render_dialog(dialog_queue[0].speaker, dialog_queue[0].text.substr(0, dialog_index));
    }
}

void Scene::render_dialog(std::string speaker, std::string text) {
    static const int ROW_CHAR_LENGTH = 37;
    static const SDL_Rect DIALOG_BOX_RECT = (SDL_Rect) { .x = 0, .y = 130, .w = 320, .h = 50 };
    static const vec2 DIALOG_PADDING = (vec2) { .x = 10, .y = 2 };
    static const int DIALOG_LINE_HEIGHT = 14;
    static const SDL_Rect SPEAKER_BOX_RECT = (SDL_Rect) { .x = 0, .y = 115, .w = 80, .h = 20 };
    static const SDL_Rect SPEAKER_TEXT_CENTER_RECT = (SDL_Rect) { .x = 0, .y = 114, .w = 80, .h = 20 };

    std::string rows[3];

    int row_index = 0;
    while(row_index != 3 && text.length() != 0) {
        std::size_t space_index = text.find(" ");
        std::string next_word;

        if(space_index == std::string::npos) {
            next_word = text;
            text = "";
        } else {
            next_word = text.substr(0, space_index);
            text = text.substr(space_index + 1);
        }

        if(rows[row_index].length() + next_word.length() + 1 > ROW_CHAR_LENGTH) {
            row_index++;
        }
        if(row_index != 3) {
            if(rows[row_index].length() != 0) {
                rows[row_index] += " ";
            }
            rows[row_index] += next_word;
        }
    }

    render_dialog_box(DIALOG_BOX_RECT);
    for(int i = 0; i < 3; i++) {
        if(rows[i].length() == 0) {
            continue;
        }
        render_text(rows[i].c_str(), FONT_HELVETICA, COLOR_BLACK,
                (vec2) {
                .x = DIALOG_BOX_RECT.x + DIALOG_PADDING.x,
                .y = DIALOG_BOX_RECT.y + DIALOG_PADDING.y + (DIALOG_LINE_HEIGHT * i) });
    }
    if(speaker.length() != 0) {
        render_dialog_box(SPEAKER_BOX_RECT);
        render_text_centered(speaker.c_str(), FONT_HELVETICA, COLOR_BLACK, SPEAKER_TEXT_CENTER_RECT);
    }
}
