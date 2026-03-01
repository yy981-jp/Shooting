#include "playScene.h"
#include "../scenes/playScene.h"

void commandExec::playScene::operator()(const cmd::enemyBezier& c) const { scene.enemyBezier_Manager->generate(vec2f(c.x,c.y),c.pattern,c.duration); }
void commandExec::playScene::operator()(const cmd::simpleBullet& c) const { scene.simpleBullet_Manager->generate(c.pos,c.degree,c.speed); }
void commandExec::playScene::operator()(const cmd::playerBullet& c) const { scene.playerBullet_Manager->generate(c.pos); }
