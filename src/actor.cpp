#include "actor.hpp"

#include "render.hpp"

const float ACTOR_FRAME_DURATION = 0.2f;

Actor::Actor(std::string image_path) {
    image_index = render_load_spritesheet(image_path, (vec2) { .x = 16, .y = 16 });
    facing_direction = DIRECTION_DOWN;
    position = (vec2) { .x = 0, .y = 0 };
    velocity = (vec2) { .x = 0,. y = 0 };
    animation_timer = 0;
    animation_frame = 0;
}

SDL_Rect Actor::get_rect() const {
    vec2 frame_size = render_get_frame_size(image_index);
    return (SDL_Rect) { .x = position.x, .y = position.y, .w = frame_size.x, .h = frame_size.y };
}

void Actor::update(float delta) {
    position += velocity;

    if(velocity.y > 0) {
        facing_direction = DIRECTION_DOWN;
    } else if(velocity.y < 0) {
        facing_direction = DIRECTION_UP;
    } else if(velocity.x > 0) {
        facing_direction = DIRECTION_RIGHT;
    } else if(velocity.x < 0) {
        facing_direction = DIRECTION_LEFT;
    }
    update_sprite(delta);
}

void Actor::handle_collision(const SDL_Rect& collider) {
    position -= velocity;
    SDL_Rect self_rect = get_rect();

    self_rect.x += velocity.x;
    bool x_caused_collision = rects_intersect(self_rect, collider);
    self_rect.x -= velocity.x;

    self_rect.y += velocity.y;
    bool y_caused_collision = rects_intersect(self_rect, collider);
    self_rect.y -= velocity.y;

    if(!x_caused_collision) {
        position.x += velocity.x;
    }
    if(!y_caused_collision) {
        position.y += velocity.y;
    }
}

void Actor::update_sprite(float delta) {
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

void Actor::render(const vec2& camera_offset) {
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
