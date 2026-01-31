#pragma once

#include <cstdint>
#include <variant>

struct command_enemyBezier {
    uint16_t x, y, pattern;
    uint32_t duration;
};

using GameCommand = std::variant<
    command_enemyBezier
>;

