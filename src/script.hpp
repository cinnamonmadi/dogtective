#pragma once

#include "vector.hpp"
#include "actor.hpp"
#include <vector>
#include <string>

typedef enum ScriptType {
    SCRIPT_MOVE,
    SCRIPT_WAITFOR,
    SCRIPT_TURN,
    SCRIPT_DELAY,
    SCRIPT_LOOP
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

class Script {
    public:
        Script(std::string path);
        bool is_finished() const;
        ScriptLine& current_line();
        void restart();
        void increment();

    private:
        std::vector<ScriptLine> lines;
        int current_line_index;
};
