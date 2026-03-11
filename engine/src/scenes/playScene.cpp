#include "playScene.h"
#include "../gcms/playScene.h"
#include "build_info.h"


PlayScene::PlayScene(SceneContext& ctx):
    player(static_cast<vec2f>(ctx.gfx->getSpriteHalfSize(SpriteID::player)/2), 5.0f*60.0f),
    simpleBullet_Manager(ctx.gfx->getSpriteHalfSize(SpriteID::simpleBullet)),
    pointBullet_Manager(ctx.gfx->getSpriteHalfSize(SpriteID::simpleBullet)),
    ui(ctx), vm(stgdatpath) {}

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
    ui.initCur();
    ui.write(FontSize::f16, std::string("build: ") + BUILD_GIT);
    ui.enter();
    ui.write(FontSize::f16, "Copyright (c) 2026 yy981");
    ui.enter();
    ui.write(FontSize::f32, "fps: ");
    ui.write(currentFps, FontSize::f32);
    ui.enter();
    ui.write(FontSize::f32, "ents: ");
    ui.write((int)entMgr.size(), FontSize::f32);
    ui.enter();
}

void PlayScene::handleCommand(const GameCommand& cmd, Game& game) {
   std::visit(commandExec::playScene{game,*this}, cmd);
}
