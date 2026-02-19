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


using GameCommand = std::variant<
    cmd::enemyBezier,
    cmd::simpleBullet,
    cmd::playerBullet
>;


class GCMS {
    std::vector<GameCommand> data;

public:
    void operator()(GameCommand c) {
        data.push_back(std::move(c));
    }

    const std::vector<GameCommand>& get() const {
        return data;
    }

    void clear() {
        data.clear();
    }
};
