#pragma once

#include "state.hpp"
#include "actor.hpp"
#include "vector.hpp"
#include "inventory.hpp"
#include "menu.hpp"
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

        typedef struct Scenery {
            SDL_Rect collider;
            std::string name;
            std::vector<DialogLine> description;
        } Scenery;

        Scene(std::string path);
        void handle_input(SDL_Event e);
        void update(float delta);
        void render();

    private:
        // Init
        void init_ui_rects();

        SDL_Rect DIALOG_BOX_RECT;
        SDL_Rect SPEAKER_BOX_RECT;
        SDL_Rect SPEAKER_TEXT_CENTER_RECT;
        SDL_Rect EVIDENCE_PROMPT_RECT;

        int background_image;
        vec2 map_size;
        std::vector<SDL_Rect> colliders;
        std::vector<Scenery> scenery;

        // Input
        void handle_movement_input(SDL_Event e);
        void handle_dialog_input(SDL_Event e);
        void handle_evidence_confirmation_input(SDL_Event e);

        bool direction_key_pressed[4];

        // Player
        void player_handle_input(float delta);
        void player_interact();

        vec2 player_direction;

        // Camera
        void camera_update(float delta);

        vec2 camera_offset;

        // Actors
        void actor_update(int actor_index, float delta);

        std::vector<Actor> actors;
        int actor_player;
        int actor_being_spoken_to;

        // Scripts
        int get_actor_from_name(std::string name);
        void script_begin(int script_index);
        void script_finish(int script_index);
        void script_execute(int script_index, float delta);

        std::vector<Script> scripts;
        int current_script;

        // Dialog
        void open_dialog(const std::vector<DialogLine>& dialog_lines);
        void progress_dialog();
        void render_dialog(std::string speaker, std::string text, std::size_t dialog_index);

        std::vector<DialogLine> dialog_queue;
        std::size_t dialog_index;
        float dialog_index_timer;
        bool dialog_open;
        int dialog_left_profile_index;
        int dialog_right_profile_index;

        // Evidence
        void evidence_dialog_handle_select();

        Menu evidence_dialog;
        std::string evidence_dialog_evidence_name;
        bool evidence_dialog_open;
};
