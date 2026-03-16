#include "playScene.h"
#include "../gcms/exec.h"

#include <y9inc/string.h>


IMPL_CMD_PLAY(cmd::bezierEnemy) { scene.bezierEnemy_Manager.generate(vec2f(c.x,c.y),c.pattern,c.duration); }
IMPL_CMD_PLAY(cmd::simpleBullet) { scene.simpleBullet_Manager.generate(c.pos,c.degree,c.speed); }
IMPL_CMD_PLAY(cmd::pointBullet) { scene.pointBullet_Manager.generate(c.pos, false /* TODO */); }
IMPL_CMD_PLAY(cmd::playerBullet) { scene.playerBullet_Manager.generate(c.pos); }
IMPL_CMD_PLAY(cmd::notiFps) { scene.currentFps = c.fps; }


PlayScene::PlayScene(SceneContext& ctx):
    player(ctx.gfx, 5.0f*60.0f),
    simpleBullet_Manager(ctx.gfx->getSpriteHalfSize(SpriteID::simpleBullet)),
    pointBullet_Manager(ctx.gfx->getSpriteHalfSize(SpriteID::simpleBullet)),
    ui(ctx,{410,-390},{600,390}), vm(stgdatpath) {
        std::ifstream ifs(Assets+"build.info");
        if (!ifs) {
            buildID = "UNKNOWN";
            buildTimeStamp = "UNKNOWN";
        } else {
            std::string line;
            std::getline(ifs, line);
            const auto&& values = st::split(line, ",");
            buildID = values[0];
            buildTimeStamp = values[1];
        }
    }

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
    ctx.gfx->drawSprite(SpriteID::background, {0,0});
    IEntityManagerBase::drawAll(ctx.gfx);
    player.draw(ctx.gfx);
    ctx.gfx->drawSprite(SpriteID::uiBackground, {500,0});
    drawUI(ctx);
    ctx.gfx->flush();

    // DEBUG
    physWorld.draw(ctx.gfx);
}

void PlayScene::drawUI(const SceneContext& ctx) const {
    ui.initCur();
    ui.write(FontSize::f16, "buildID:");
    ui.enter();
    ui.write(FontSize::f16, buildID);
    ui.enter();
    ui.write(FontSize::f16, "buildTime:");
    ui.enter();
    ui.write(FontSize::f16, buildTimeStamp);
    ui.enter(2);
    ui.write(FontSize::f16, "Copyright (c) 2026 yy981");
    ui.setSize(FontSize::f32);
    ui.enter();
    ui.write(FontSize::f32, "fps: ");
    ui.write(currentFps, FontSize::f32);
    ui.enter();
    ui.write(FontSize::f32, "ents: ");
    ui.write((int)entMgr.size(), FontSize::f32);
    ui.enter();
    ui.write(FontSize::f32, "score: ");
    ui.write((int)player.score, FontSize::f32);
    ui.enter();
    ui.write(FontSize::f32, "RL: ");
    ui.write((int)player.remainingLives, FontSize::f32);
    ui.enter();
}

void PlayScene::handleCommand(const GameCommand& cmd, Game& game) {
   std::visit(commandExec::Play{game,*this}, cmd);
}
