#include "map.hpp"

#include "render.hpp"

Map::Map(std::string filename) {
    background_filepath = filename;
    background_image = render_load_image(filename.c_str());
}

void Map::render() {
    render_image(background_image, (vec2) { .x = 0, .y = 0 });
}
