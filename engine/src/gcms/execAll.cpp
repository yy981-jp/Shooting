#include "playScene.h"
#include "../scenes/playScene.h"

void commandExec::playScene::operator()(const cmd::bezierEnemy& c) const { scene.bezierEnemy_Manager.generate(vec2f(c.x,c.y),c.pattern,c.duration); }
void commandExec::playScene::operator()(const cmd::simpleBullet& c) const { scene.simpleBullet_Manager.generate(c.pos,c.degree,c.speed); }
void commandExec::playScene::operator()(const cmd::pointBullet& c) const { scene.pointBullet_Manager.generate(c.pos, false /* TODO */); }
void commandExec::playScene::operator()(const cmd::playerBullet& c) const { scene.playerBullet_Manager.generate(c.pos); }
