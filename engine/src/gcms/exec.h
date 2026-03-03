#pragma once
#include "gcms.h"
#include "../core/Game.h"

namespace commandExec {

struct base {
    Game& game;

    void operator()(const cmd::sfx& c) const { game.playSfx(c.id); }
    void operator()(const cmd::changeScene& c) const { game.setScene(c.id); }
};

}
