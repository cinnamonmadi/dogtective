#pragma once

#include "state.hpp"
#include "actor.hpp"
#include "vector.hpp"
#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Scene : public IState {
    public:
        typedef enum ScriptType {
            SCRIPT_MOVE,
            SCRIPT_WAITFOR,
            SCRIPT_TURN,
            SCRIPT_DELAY,
            SCRIPT_DIALOG,
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

        typedef struct ScriptDialog {
            std::vector<DialogLine> lines;
            bool has_been_opened;
        } ScriptDialog;

        typedef struct ScriptLine {
            ScriptType type;
            ScriptMove move;
            ScriptWaitFor waitfor;
            ScriptTurn turn;
            ScriptDelay delay;
            ScriptDialog dialog;
        } ScriptLine;

        typedef struct Script {
            std::vector<ScriptLine> lines;
            std::vector<std::string> required_actors;
            int current_line;
            bool playing;
        } Script;

        Scene(std::string path);
        void handle_input(SDL_Event e);
        void update(float delta);
        void render();

    private:
        // Player
        void player_handle_input(float delta);
        void player_interact();

        // Actors
        void actor_update(int actor_index, float delta);

        // Scripts
        int get_actor_from_name(std::string name);
        void script_begin(int script_index);
        void script_finish(int script_index);
        void script_execute(int script_index, float delta);

        // Dialog
        void open_dialog(const std::vector<DialogLine>& dialog_lines);
        void render_dialog(std::string speaker, std::string text);

        bool direction_key_pressed[4];
        vec2 player_direction;
        vec2 camera_offset;

        int background_image;
        std::vector<SDL_Rect> colliders;

        std::vector<Actor> actors;
        int actor_player;

        std::vector<DialogLine> dialog_queue;
        std::size_t dialog_index;
        float dialog_index_timer;
        bool dialog_open;

        std::vector<Script> scripts;
};
