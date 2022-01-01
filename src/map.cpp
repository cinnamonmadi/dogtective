#include "map.hpp"

#include <iostream>

Map::Map() {
    background_image = IMAGE_MAP;
    camera_offset = (vec2) { .x = 0, .y = 0 };

    player_direction = (vec2) { .x = 0, .y = 0 };
    player = Actor(IMAGE_PLAYER);
    for(int i = 0; i < 4; i++) {
        direction_key_pressed[i] = false;
    }
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
    player.velocity = player_direction;
    player.position += player.velocity;

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

void Map::render() {
    render_image(background_image, camera_offset.inverse());
    player.render(camera_offset);
}

// Actor functions

const float ACTOR_FRAME_DURATION = 0.2f;

Map::Actor::Actor() {
}

Map::Actor::Actor(ImageName image_name) {
    this->image_name = image_name;
    facing_direction = DIRECTION_DOWN;
    position = (vec2) { .x = 0, .y = 0 };
    velocity = (vec2) { .x = 0,. y = 0 };
    animation_timer = 0;
    animation_frame = 0;

    update_sprite(0);
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

    render_image_frame(image_name, sprite_frame, position - camera_offset, flipped);
}
