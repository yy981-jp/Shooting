#pragma once

#include "vec2.h"

#include <cstdint>
#include <variant>


namespace cmd {
    struct enemyBezier { int x, y, pattern, duration; };
    struct simpleBullet {
        vec2f pos;
        int rotate;
        float speed;
    };
    struct playerBullet { vec2f pos; };
}


using GameCommand_core = std::variant<
    cmd::enemyBezier,
    cmd::simpleBullet,
    cmd::playerBullet
>;


struct GameCommand {
    bool enable;
    operator bool() { return enable; }
    GameCommand_core c;
    GameCommand& operator=(const GameCommand_core& i_c) {
        c = i_c;
        enable = true;
        return *this;
    }
    GameCommand(): enable(false) {}
    GameCommand(const GameCommand_core& c): enable(true), c(c) {}
};

using GameCommands = std::vector<GameCommand>;
