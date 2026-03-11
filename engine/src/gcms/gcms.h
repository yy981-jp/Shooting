#pragma once

#include "../core/vec2.h"

#include <cstdint>
#include <variant>

#include "../audio/sfx.h"
#include "../core/hitEvent.h"

namespace cmd {
    struct bezierEnemy { int x, y, pattern, duration; };
    struct simpleBullet {
        vec2f pos;
        int degree;
        float speed;
    };
    struct pointBullet { vec2f pos; };
    struct playerBullet { vec2f pos; };
    struct sfx { SFXID id; };
    struct changeScene { SceneID id; };
    struct onHit { HitEvents events; };
    struct notiFps { float fps; };
}


using GameCommand = std::variant<
    cmd::bezierEnemy,
    cmd::simpleBullet,
    cmd::pointBullet,
    cmd::playerBullet,
    cmd::sfx,
    cmd::changeScene,
    cmd::onHit,
    cmd::notiFps
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
