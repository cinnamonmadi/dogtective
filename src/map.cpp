#include "map.hpp"

Map::Map() {
    background_image = IMAGE_MAP;
    camera_offset = (vec2) { .x = 0, .y = 0 };

    player = Actor(IMAGE_PLAYER);
}

void Map::update(float delta) {
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
