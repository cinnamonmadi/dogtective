#pragma once

#include "vector.hpp"
#include <string>

class Map {
    public:
        Map(std::string filename);
        void render();

        vec2 camera_offset;
    private:
        std::string background_filepath;
        int background_image;
};
