#pragma once

#include "state.hpp"
#include "actor.hpp"
#include "vector.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Scene : public IState {
    public:
        /*
        typedef enum ScriptType {
            SCRIPT_MOVE,
            SCRIPT_WAITFOR,
            SCRIPT_TURN,
            SCRIPT_DELAY,
        } ScriptType;

        typedef struct ScriptMove {
            std::string actor;
            vec2 target;
        } ScriptMove;

        typedef struct ScriptWaitFor {
            std::string actor;
        } ScriptWait;

        typedef struct ScriptTurn {
            std::string actor;
            Direction direction;
        } ScriptTurn;

        typedef struct ScriptDelay {
            float duration;
            float timer;
        } ScriptDelay;

        typedef struct ScriptLine {
            ScriptType type;
            ScriptMove move;
            ScriptWaitFor waitfor;
            ScriptTurn turn;
            ScriptDelay delay;
        } ScriptLine;

        typedef struct Script {
            std::vector<ScriptLine> lines;
            std::vector<std::string> required_actors;
            int current_line;

            Script(std::string path);
        } Script;
        */

        Scene(std::string path);
        void handle_input(SDL_Event e);
        void update(float delta);
        void render();

    private:
        // int get_actor_from_name(std::string name);
        // void script_execute(int script_index, float delta);
        void player_interact();

        bool direction_key_pressed[4];
        vec2 player_direction;
        vec2 camera_offset;

        int background_image;
        std::vector<SDL_Rect> colliders;

        std::vector<Actor> actors;
        int actor_player;

        // std::vector<Script> scripts;
};
