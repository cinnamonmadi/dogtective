#include "pause.hpp"

#include "render.hpp"
#include "vector.hpp"
#include "inventory.hpp"
#include <string>

const int MAIN_MENU_LENGTH = 3;
const std::string MAIN_MENU_LABELS[MAIN_MENU_LENGTH] = {"Evidence", "Save", "Quit"};

Pause::Pause() {
    render_previous = true;

    Menu main_menu = Menu((SDL_Rect) {
        .x = SCREEN_WIDTH - 100,
        .y = 0,
        .w = 100,
        .h = SCREEN_HEIGHT
    });
    main_menu.labels.push_back("Evidence");
    main_menu.labels.push_back("Save");
    main_menu.labels.push_back("Quit");

    menus.push_back(main_menu);
}

void Pause::open_evidence_menu() {
    Menu evidence_menu = Menu((SDL_Rect) {
        .x = SCREEN_WIDTH - 200,
        .y = 0,
        .w = 200,
        .h = SCREEN_HEIGHT
    });

    for(int i = 0; i < evidence.size(); i++) {
        if(evidence[i].registered) {
            evidence_menu.labels.push_back(evidence[i].name);
        }
    }

    menus.push_back(evidence_menu);
}

void Pause::handle_input(SDL_Event e) {
    if(e.type == SDL_KEYDOWN) {
        SDL_Keycode keycode = e.key.keysym.sym;

        switch(keycode) {
            case SDLK_w: {
                get_current_menu()->navigate_up();
                break;
            }
            case SDLK_s: {
                get_current_menu()->navigate_down();
                break;
            }
            case SDLK_SPACE: {
                menu_select();
                break;
            }
            case SDLK_x: {
                menu_back();
                break;
            }
            case SDLK_p: {
                finished = true;
                break;
            }
        }
    }
}

void Pause::menu_select() {
    PauseMenuState state = get_state();
    std::string label = get_current_menu()->select();

    if(state == PAUSE_MENU_MAIN && label == "Quit") {
        finished = true; // TODO make this quit the whole game not just the state
    } else if(state == PAUSE_MENU_MAIN && label == "Evidence") {
        open_evidence_menu();
    }
}

void Pause::menu_back() {
    if(get_state() == PAUSE_MENU_MAIN) {
        finished = true; // Quit the Pause state
    } else {
        menus.pop_back();
    }
}

PauseMenuState Pause::get_state() const {
    return (PauseMenuState)(menus.size() - 1);
}

Menu* Pause::get_current_menu() {
    return &menus[menus.size() - 1];
}

void Pause::update(float delta) {

}

void Pause::render() {
    get_current_menu()->render();
}
