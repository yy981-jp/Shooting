#include "playScene.h"
#include "../gcms/playScene.h"


PlayScene::PlayScene(SceneContext& ctx):
    player(static_cast<vec2f>(ctx.gfx->getSpriteHalfSize(SpriteID::player)/2), 5.0f*60.0f),
    simpleBullet_Manager(ctx.gfx->getSpriteHalfSize(SpriteID::simpleBullet)),
    vm(stgdatpath) {}

void PlayScene::update(SceneContext& ctx, const float dt) {
    // VM step
    if (vm.running) vm.step(*ctx.gcms);

    // 当たり判定
    physWorld.step(*ctx.gcms);

    // entity update
    vec2i d = makeDir(*ctx.input);
    player.update(dt, *ctx.gcms, d.x, d.y, has(*ctx.input, SHTKeyCode::shift), has(*ctx.input, SHTKeyCode::z));
    // if (!player->isAllive()) running = false;
    IEntityManagerBase::updateAll(dt,*ctx.gcms);
}

void PlayScene::draw(const SceneContext& ctx) const {
    ctx.gfx->drawSpriteNow(SpriteID::background, {0,0});
    player.draw(ctx.gfx);
    IEntityManagerBase::drawAll(ctx.gfx);

    // DEBUG
    // physWorld.draw(ctx.gfx);
}

void PlayScene::handleCommand(const GameCommand& cmd, Game& game) {
   std::visit(commandExec::playScene{game,*this}, cmd);
}
