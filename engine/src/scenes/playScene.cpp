#include "playScene.h"
#include "../gcms/playScene.h"


void PlayScene::handleCommand(const GameCommand& cmd, Game& game) {
   std::visit(commandExec::playScene{game,*this}, cmd);
}
