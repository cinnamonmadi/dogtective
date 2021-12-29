#pragma once

#include "state.hpp"

class MapState : public IState {
    public:
        void update();
        void render();
};
