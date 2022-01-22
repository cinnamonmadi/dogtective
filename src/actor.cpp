#include "actor.hpp"

#include "render.hpp"

Direction get_direction_from_name(std::string name) {
    static const std::string direction_names[4] = {"up", "right", "down", "left"};
    for(int i = 0; i < 4; i++) {
        if(name == direction_names[i]) {
            return (Direction)i;
        }
    }

    std::cout << "Error in get_direction_from_name()! Direction " << name << " is not a real direction!" << std::endl;
    return DIRECTION_UP;
}

// Actor functions

const float ACTOR_FRAME_DURATION = 0.1f;
const int SPEED = 1;

Actor::Actor(std::string name, std::string image_path_prefix) {
    this->name = name;

    image_profile_index = render_load_image(image_path_prefix + "_profile.png");
    image_idle_index = render_load_spritesheet(image_path_prefix + "_idle.png", (vec2) { .x = 32, .y = 32 });
    image_walk_index = render_load_spritesheet(image_path_prefix + "_walk.png", (vec2) { .x = 32, .y = 32 });
    image_index = image_idle_index;
    image_flipped = false;

    facing_direction = DIRECTION_DOWN;
    position = (vec2) { .x = 0, .y = 0 };
    velocity = (vec2) { .x = 0,. y = 0 };

    path_index = 0;
    path_wait_timer = 0;

    animation_timer = 0;
    animation_frame = 0;

    in_scene = false;
    target = (vec2) { .x = -1, .y = -1 };
}

SDL_Rect Actor::get_rect() const {
    vec2 frame_size = render_get_frame_size(image_index);
    return (SDL_Rect) { .x = position.x, .y = position.y, .w = frame_size.x, .h = frame_size.y };
}

bool Actor::has_target() const {
    return target.x != -1;
}

void Actor::update(float delta) {
    if(in_scene) {
        if(has_target()) {
            if(position.x == target.x && position.y == target.y) {
                target = (vec2) { .x = -1, .y = -1 };
                velocity = (vec2) { .x = 0, .y = 0 };
            } else {
                set_velocity_towards(target);
            }
        } else {
            velocity = (vec2) { .x = 0, .y = 0 };
        }
    } else if(path.size() != 0) {
        if(position != path[path_index].position) {
            set_velocity_towards(path[path_index].position);
        } else if(path_wait_timer > 0) {
            velocity = (vec2) { .x = 0, .y = 0 };
            path_wait_timer -= delta;
            facing_direction = path[path_index].direction;
        } else {
            velocity = (vec2) { .x = 0, .y = 0 };
            path_index = (path_index + 1) % path.size();
            path_wait_timer = path[path_index].wait_duration;
        }
    }

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

void Actor::set_velocity_towards(vec2 target_position) {
    if(position.x < target_position.x) {
        velocity.x = SPEED;
    } else if(position.x > target_position.x) {
        velocity.x = -SPEED;
    } else {
        velocity.x = 0;
    }

    if(position.y < target_position.y) {
        velocity.y = SPEED;
    } else if(position.y > target_position.y) {
        velocity.y = -SPEED;
    } else {
        velocity.y = 0;
    }
}

void Actor::set_direction_towards(vec2 target_position) {
    if(abs(position.x - target_position.x) >= abs(position.y - target_position.y)) {
        if(position.x >= target_position.x) {
            facing_direction = DIRECTION_LEFT;
        } else {
            facing_direction = DIRECTION_RIGHT;
        }
    } else {
        if(position.y >= target_position.y) {
            facing_direction = DIRECTION_UP;
        } else {
            facing_direction = DIRECTION_DOWN;
        }
    }
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
        image_index = image_idle_index;
        image_flipped = false;
        animation_timer = ACTOR_FRAME_DURATION;
        switch(facing_direction) {
            case DIRECTION_UP:
                animation_frame = 3;
                break;
            case DIRECTION_RIGHT:
                animation_frame = 0;
                break;
            case DIRECTION_DOWN:
                animation_frame = 1;
                break;
            case DIRECTION_LEFT:
                animation_frame = 2;
                break;
        }
    } else {
        image_index = image_walk_index;
        animation_timer -= delta;
        if(animation_timer <= 0) {
            animation_timer += ACTOR_FRAME_DURATION;
            animation_frame = (animation_frame + 1) % 8; // TODO change this to rely on frame data rather than a hard coded frame count
        }
        image_flipped = facing_direction == DIRECTION_LEFT;
    }
}

void Actor::render(const vec2& camera_offset) {
    vec2 sprite_frame = (vec2) { .x = animation_frame, .y = 0 };

    render_image_frame(image_index, sprite_frame, position - camera_offset, image_flipped);
}
