#include "playScene.h"
#include "../gcms/playScene.h"


PlayScene::PlayScene(SceneContext& ctx):
    player(static_cast<vec2f>(ctx.gfx->getSpriteHalfSize(SpriteID::player)/2), 5.0f*60.0f),
    simpleBullet_Manager(ctx.gfx->getSpriteHalfSize(SpriteID::simpleBullet)),
    pointBullet_Manager(ctx.gfx->getSpriteHalfSize(SpriteID::simpleBullet)),
    vm(stgdatpath) {}

void PlayScene::update(SceneContext& ctx, const float dt) {
    // DEBUG
    if (has(*ctx.input, SHTKeyCode::x)) (*ctx.gcms)(cmd::changeScene{SceneID::title});

    // VM step
    if (vm.running) vm.step(*ctx.gcms);

    // 当たり判定
    physWorld.step(*ctx.gcms);

    // entity update
    vec2i d = makeDir(*ctx.input);
    player.update(dt, *ctx.gcms, d.x, d.y, has(*ctx.input, SHTKeyCode::shift), has(*ctx.input, SHTKeyCode::z));
    if (!player.isAllive()) (*ctx.gcms)(cmd::changeScene{SceneID::title});
    IEntityManagerBase::updateAll(dt,*ctx.gcms);
}

void PlayScene::draw(const SceneContext& ctx) const {
    ctx.gfx->drawSpriteNow(SpriteID::background, {0,0});
    ctx.gfx->drawSpriteNow(SpriteID::uiBackground, {500,0});
    player.draw(ctx.gfx);
    IEntityManagerBase::drawAll(ctx.gfx);
    drawUI(ctx);

    // DEBUG
    // physWorld.draw(ctx.gfx);
}

void PlayScene::drawUI(const SceneContext& ctx) const {
    ui.init();
    ui.write(ctx, FontSize::f16, "build: " + std::string(__DATE__) + " " + std::string(__TIME__));
    ui.write(ctx, FontSize::f16, "Copyright (c) 2026 yy981");
}

void PlayScene::handleCommand(const GameCommand& cmd, Game& game) {
   std::visit(commandExec::playScene{game,*this}, cmd);
}
