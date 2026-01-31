#pragma once

#include <cstdint>
#include <variant>

struct command_enemyBezier {
    int16_t x, y;
    uint16_t pattern;
    uint32_t duration;
};

using GameCommand = std::variant<
    command_enemyBezier
>;

