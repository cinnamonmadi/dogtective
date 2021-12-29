#include "map.hpp"
#include "vector.hpp"
#include "render.hpp"

void MapState::update() {

}

void MapState::render() {
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            render_image(IMAGE_TILESET, (vec2) { .x = 0, .y = 0 }, (vec2) { .x = x * 16, .y = y * 16 });
        }
    }
}
