#pragma once

#include "../core/vec2.h"

#include <cstdint>
#include <variant>

#include "../audio/sfx.h"
#include "../core/hitEvent.h"


/*
namespace cmd {
    struct sfx { SFXID id; };
    struct changeScene { SceneID id; };
    struct onHit { HitEvents events; };

    struct bezierEnemy { int x, y, pattern, duration; };
    struct simpleBullet {
        vec2f pos;
        int degree;
        float speed;
    };
    struct pointBullet { vec2f pos; };
    struct playerBullet { vec2f pos; };
    struct notiFps { float fps; };
}
*/

namespace cmd {
#define GLOBAL(name, body) struct name { body };
#define PLAY(name, body) struct name { body };
#include "../../../shared/cmd.def"
#undef GLOBAL
#undef PLAY
    struct _ {};
}


using GameCommand = std::variant<
#define GLOBAL(name, body) cmd::name,
#define PLAY(name, body) cmd::name,
#include "../../../shared/cmd.def"
#undef GLOBAL
#undef PLAY
    cmd::_
>;


class GCMS {
    std::vector<GameCommand> data;

public:
    template<class T> void operator()(T&& c) {
        data.emplace_back(std::forward<T>(c));
    }
    
    const std::vector<GameCommand>& get() const {
        return data;
    }

    void clear() {
        data.clear();
    }
};
