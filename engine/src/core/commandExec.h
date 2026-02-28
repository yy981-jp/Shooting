#pragma once
#include "Game.h"


struct commandExec_core {
    Game& game;

    void operator()(const cmd::enemyBezier& c) const { game.enemyBezier_Manager->generate(vec2f(c.x,c.y),c.pattern,c.duration); }
    void operator()(const cmd::simpleBullet& c) const { game.simpleBullet_Manager->generate(c.pos,c.degree,c.speed); }
    void operator()(const cmd::playerBullet& c) const { game.playerBullet_Manager->generate(c.pos); }
    void operator()(const cmd::sfx& c) const { game.playSfx(c.id); }
};
