#include "menu.hpp"

#include "render.hpp"

Menu::Menu() {}

Menu::Menu(SDL_Rect rect) {
    this->rect = rect;
    cursor_index = 0;
    cursor_image = render_load_image("./res/cursor.png");

    padding_left = 8;
    padding_top = 5;

    reset();
}

void Menu::reset() {
    cursor_index = 0;
}

void Menu::navigate_up() {
    cursor_index--;
    if(cursor_index < 0) {
        cursor_index = labels.size() - 1;
    }
}

void Menu::navigate_down() {
    cursor_index++;
    if(cursor_index == labels.size()) {
        cursor_index = 0;
    }
}

std::string Menu::select() {
    return labels[cursor_index];
}

void Menu::render() {
    static const int LINE_HEIGHT = 16;
    render_dialog_box(rect);
    for(int i = 0; i < labels.size(); i++) {
        render_text(labels[i].c_str(), FONT_HELVETICA, COLOR_BLACK, (vec2) { .x = rect.x + padding_left + 12, .y = rect.y + padding_top + (LINE_HEIGHT * i) });
    }
    render_image(cursor_image, (vec2) { .x = rect.x + padding_left, .y = rect.y + padding_top + (cursor_index * LINE_HEIGHT) + 4 });
}
