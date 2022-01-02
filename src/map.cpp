#include "map.hpp"

#include "render.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>

using nlohmann::json;

Map::Map() {
    camera_offset = (vec2) { .x = 0, .y = 0 };
    player = Actor("./res/witch.png");
}

bool Map::load(std::string path) {
    std::ifstream map_file;
    map_file.open(path);
    if(!map_file.is_open()) {
        std::cout << "Unable to open file!" << std::endl;
        return false;
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

    return true;
}

bool Map::save(std::string path) {
    json collider_arrays = json::array();
    for(SDL_Rect collider : colliders) {
        json collider_array = json::array();
        collider_array.insert(collider_array.end(), collider.x);
        collider_array.insert(collider_array.end(), collider.y);
        collider_array.insert(collider_array.end(), collider.w);
        collider_array.insert(collider_array.end(), collider.h);
        collider_arrays.insert(collider_arrays.end(), collider_array);
    }

    std::cout << render_get_path(background_image) << std::endl;
    json map_json = {
        {"background", render_get_path(background_image)},
        {"colliders", collider_arrays}
    };
    std::ofstream map_file;
    map_file.open(path);
    if(!map_file.is_open()) {
        std::cout << "Unable to open file!" << std::endl;
        return false;
    }
    map_file << map_json.dump(4);
    map_file.close();

    return true;
}

void Map::update(float delta) {
    player.position += player.velocity;
    actor_handle_collisions(player);

    if(player.velocity.y > 0) {
        player.facing_direction = DIRECTION_DOWN;
    } else if(player.velocity.y < 0) {
        player.facing_direction = DIRECTION_UP;
    } else if(player.velocity.x > 0) {
        player.facing_direction = DIRECTION_RIGHT;
    } else if(player.velocity.x < 0) {
        player.facing_direction = DIRECTION_LEFT;
    }
    player.update_sprite(delta);
}

// Ideas: we could possibly have two functions, a simple collisions function and one like the one below
// that takes direction into account. That way we can use the simple collisions on pacing NPCs and save
// the nicer feeling but more expensive sliding collision for the player movement

void Map::actor_handle_collisions(Map::Actor& actor) {
    for(SDL_Rect collider : colliders) {
        if(rects_intersect(actor.get_rect(), collider)) {
            bool x_caused = false;
            bool y_caused = false;

            actor.position -= actor.velocity;

            // Recheck collision with just X movement
            actor.position.x += actor.velocity.x;
            if(rects_intersect(actor.get_rect(), collider)) {
                x_caused = true;
            }
            actor.position.x -= actor.velocity.x;

            // Recheck collision with just Y movement
            actor.position.y += actor.velocity.y;
            if(rects_intersect(actor.get_rect(), collider)) {
                y_caused = true;
            }
            actor.position.y -= actor.velocity.y;

            if(!x_caused) {
                actor.position.x += actor.velocity.x;
            }
            if(!y_caused){
                actor.position.y += actor.velocity.y;
            }
        }
    }
}

void Map::render() {
    render_image(background_image, camera_offset.inverse());
    player.render(camera_offset);
}

// Actor functions

const float ACTOR_FRAME_DURATION = 0.2f;

Map::Actor::Actor() {
}

Map::Actor::Actor(std::string image_path) {
    image_index = render_load_spritesheet(image_path, (vec2) { .x = 16, .y = 16 });
    facing_direction = DIRECTION_DOWN;
    position = (vec2) { .x = 0, .y = 0 };
    velocity = (vec2) { .x = 0,. y = 0 };
    animation_timer = 0;
    animation_frame = 0;

    update_sprite(0);
}

SDL_Rect Map::Actor::get_rect() const {
    // TODO do something better for the width and height here. Do they have sprite based colliders? Or do we define them somewhere?
    return (SDL_Rect) { .x = position.x, .y = position.y, .w = 16, .h = 16 };
}

void Map::Actor::update_sprite(float delta) {
    if(velocity.x == 0 && velocity.y == 0) {
        animation_timer = ACTOR_FRAME_DURATION;
        animation_frame = 0;
    } else {
        animation_timer -= delta;
        if(animation_timer <= 0) {
            animation_timer += ACTOR_FRAME_DURATION;
            animation_frame = (animation_frame + 1) % 4;
        }
    }
}

void Map::Actor::render(vec2 camera_offset) {
    vec2 sprite_frame;
    switch(facing_direction) {
        case DIRECTION_DOWN:
            sprite_frame.y = 0;
            break;
        case DIRECTION_UP:
            sprite_frame.y = 1;
            break;
        default:
            sprite_frame.y = 2;
            break;
    }
    sprite_frame.x = animation_frame;
    bool flipped = facing_direction == DIRECTION_LEFT;

    render_image_frame(image_index, sprite_frame, position - camera_offset, flipped);
}
