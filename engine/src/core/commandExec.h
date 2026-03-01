#pragma once
#include "Game.h"

namespace commandExec {

struct base {
    Game& game;

    void operator()(const cmd::sfx& c) const { game.playSfx(c.id); }
};

}
