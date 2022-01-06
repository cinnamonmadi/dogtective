#include "script.hpp"

#include <iostream>
#include <fstream>

Script::Script(std::string path) {
    std::ifstream script_file;
    script_file.open(path);

    if(!script_file.is_open()) {
        std::cout << "Unable to open script file " << path << std::endl;
        return;
    }

    // Used to turn the parsed text into one of the four direction indexes
    static std::string direction_names[4] = { "up", "right", "down", "left" };

    std::string script_line;
    while(getline(script_file, script_line)) {
        ScriptLine new_line;

        // Split the line by spaces
        std::string to_parse = script_line;
        std::vector<std::string> parts;
        while(to_parse.length() != 0) {
            std::size_t space_index = to_parse.find(" ");
            if(space_index == std::string::npos) {
                parts.push_back(to_parse);
                to_parse = "";
            } else {
                parts.push_back(to_parse.substr(0, space_index));
                to_parse = to_parse.substr(space_index + 1);
            }
        }

        // Parse the line
        if(parts[0] == "move") {
            new_line.type = SCRIPT_MOVE;
            new_line.move.actor = parts[1];
            new_line.move.target = (vec2) {
                .x = std::stoi(parts[2]),
                .y = std::stoi(parts[3])
            };
        } else if(parts[0] == "waitfor") {
            new_line.type = SCRIPT_WAITFOR;
            new_line.waitfor.actor = parts[1];
        } else if(parts[0] == "turn") {
            new_line.type = SCRIPT_TURN;
            new_line.turn.actor = parts[1];
            for(int i = 0; i < 4; i++) {
                if(parts[2] == direction_names[i]) {
                    new_line.turn.direction = (Direction)i;
                }
            }
        } else if(parts[0] == "delay") {
            new_line.type = SCRIPT_DELAY;
            new_line.delay.duration = std::stof(parts[1]);
            new_line.delay.timer = new_line.delay.duration;
        } else if(parts[0] == "loop") {
            new_line.type = SCRIPT_LOOP;
        }

        // Add the parsed line to the script
        lines.push_back(new_line);
    }
}

bool Script::is_finished() const {
    return current_line_index == lines.size();
}

ScriptLine& Script::current_line() {
    return lines[current_line_index];
}

void Script::restart() {
    current_line_index = 0;
}

void Script::increment() {
    current_line_index++;
}
