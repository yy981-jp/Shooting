#include "playScene.h"
#include "../gcms/playScene.h"


PlayScene::PlayScene(SceneContext& ctx):
    player(static_cast<vec2f>(ctx.gfx->getSpriteHalfSize(SpriteID::player)/2), 5.0f*60.0f),
    playerBullet_Manager(static_cast<vec2f>(ctx.gfx->getSpriteHalfSize(SpriteID::playerBullet)/2)),
    simpleBullet_Manager(static_cast<vec2f>(ctx.gfx->getSpriteHalfSize(SpriteID::simpleBullet)/2)),
    vm(stgdatpath) {}

void PlayScene::update(SceneContext& ctx, const float dt) {
    // VM step
    if (vm.running) vm.step(*ctx.gcms);

    // entity update
    vec2i d = makeDir(*ctx.input);
    player.update(dt, *ctx.gcms, d.x, d.y, has(*ctx.input, SHTKeyCode::shift), has(*ctx.input, SHTKeyCode::z));
    // if (!player->isAllive()) running = false;
    playerBullet_Manager.update(dt);
    bezierEnemy_Manager.update(dt,*ctx.gcms);
    simpleBullet_Manager.update(dt);
}

void PlayScene::draw(const SceneContext& ctx) const {
    ctx.gfx->drawSpriteNow(SpriteID::background, {0,0});
    player.draw(ctx.gfx);
    playerBullet_Manager.draw(ctx.gfx);
    bezierEnemy_Manager.draw(ctx.gfx);
    simpleBullet_Manager.draw(ctx.gfx);
}

void PlayScene::handleCommand(const GameCommand& cmd, Game& game) {
   std::visit(commandExec::playScene{game,*this}, cmd);
}
