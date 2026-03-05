#include "playScene.h"
#include "../gcms/playScene.h"


PlayScene::PlayScene(SceneContext& ctx) {
    player = new Player(static_cast<vec2f>(ctx.gfx->getSpriteHalfSize(SpriteID::player)/2), 5.0f*60.0f);
    playerBullet_Manager = new PlayerBullet_Manager(static_cast<vec2f>(ctx.gfx->getSpriteHalfSize(SpriteID::playerBullet)/2));
    enemyBezier_Manager = new EnemyBezier_Manager(static_cast<vec2f>(ctx.gfx->getSpriteHalfSize(SpriteID::enemyBezier)/2));
    simpleBullet_Manager = new SimpleBullet_Manager(static_cast<vec2f>(ctx.gfx->getSpriteHalfSize(SpriteID::simpleBullet)/2));
    // VM
    vm = new VM(stgdatpath);
}

void PlayScene::update(SceneContext& ctx, const float dt) {
        // VM step
    if (vm->running) {
        switch (vm->step(*ctx.gcms)) {
            using enum VM::ReturnCode;
            case success: break;
            case error: throw std::runtime_error("VMで何らかの異常が発生しました"); break;
        }
    }

    // entity update
    vec2i d = makeDir(*ctx.input);
    player->update(dt, *ctx.gcms, d.x, d.y, has(*ctx.input, SHTKeyCode::shift), has(*ctx.input, SHTKeyCode::z));
    // if (!player->isAllive()) running = false;
    playerBullet_Manager->update(dt);
    enemyBezier_Manager->update(dt,*ctx.gcms);
    simpleBullet_Manager->update(dt);
}

void PlayScene::draw(const SceneContext& ctx) const {
    ctx.gfx->drawSpriteNow(SpriteID::background, {0,0});
    player->draw(ctx.gfx);
    playerBullet_Manager->draw(ctx.gfx);
    enemyBezier_Manager->draw(ctx.gfx);
    simpleBullet_Manager->draw(ctx.gfx);
}

void PlayScene::handleCommand(const GameCommand& cmd, Game& game) {
   std::visit(commandExec::playScene{game,*this}, cmd);
}
