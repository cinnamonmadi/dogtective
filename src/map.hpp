#pragma once

#include <string>

class Map {
    public:
        Map(std::string filename);
        void render();
    private:
        std::string background_filepath;
        int background_image;
};
