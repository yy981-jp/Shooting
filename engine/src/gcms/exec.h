#pragma once
#include "gcms.h"
#include "../core/Game.h"
#include "../core/entityManager.h"
#include "../core/collider.h"


struct PlayScene;


namespace commandExec {

struct Global {
    Game& game;

#define GLOBAL(name, body) void operator()(const cmd::name& c) const;
#define PLAY(name, body)
#include "../../../shared/cmd.def"
#undef GLOBAL
#undef PLAY
    void operator()(const cmd::_& c) const;
};

struct Play: public Global {
    using Global::operator();
    PlayScene& scene;

#define GLOBAL(name, body)
#define PLAY(name, body) void operator()(const cmd::name& c) const;
#include "../../../shared/cmd.def"
#undef GLOBAL
#undef PLAY
};

}

#define IMPL_CMD_PLAY(name) \
void commandExec::Play::operator()(const name& c) const

#define IMPL_CMD_GLOBAL(name) \
void commandExec::Global::operator()(const name& c) const
