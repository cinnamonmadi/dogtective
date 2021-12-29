#pragma once

class IState {
    public:
        virtual void update() = 0;
        virtual void render() = 0;
};
