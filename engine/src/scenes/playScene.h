#pragma once

#include "../VM/VM.h"
#include "../player/player.h"
#include "../bullet/playerBullet.h"
#include "../bullet/simpleBullet.h"
#include "../enemy/enemyBezier.h"

#include "scene.h"



inline vec2i makeDir(uint8_t keys) {
    return {
        (has(keys, SHTKeyCode::right) ? 1 : 0)
      - (has(keys, SHTKeyCode::left)  ? 1 : 0),

        (has(keys, SHTKeyCode::down)  ? 1 : 0)
      - (has(keys, SHTKeyCode::up)    ? 1 : 0)
    };
}

struct PlayScene: public IScene {
    const std::string stgdatpath = Assets + "main.stg.dat";

    VM* vm;

    Player* player;
    PlayerBullet_Manager* playerBullet_Manager;
    EnemyBezier_Manager* enemyBezier_Manager;
    SimpleBullet_Manager* simpleBullet_Manager;

    PlayScene(SceneContext& ctx) {
        player = new Player(static_cast<vec2f>(ctx.gfx->getSpriteSize(EntityType::player)/2), 5.0f*60.0f);
        playerBullet_Manager = new PlayerBullet_Manager(static_cast<vec2f>(ctx.gfx->getSpriteSize(EntityType::playerBullet)/2));
        enemyBezier_Manager = new EnemyBezier_Manager(static_cast<vec2f>(ctx.gfx->getSpriteSize(EntityType::enemyBezier)/2));
        simpleBullet_Manager = new SimpleBullet_Manager(static_cast<vec2f>(ctx.gfx->getSpriteSize(EntityType::simpleBullet)/2));
        // VM
        vm = new VM(stgdatpath);
    }

    void update(SceneContext& ctx, const float dt) override {
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

    void draw(const SceneContext& ctx) const override {
        player->draw(ctx.gfx);
        playerBullet_Manager->draw(ctx.gfx);
        enemyBezier_Manager->draw(ctx.gfx);
        simpleBullet_Manager->draw(ctx.gfx);
    }

    void handleCommand(const GameCommand& cmd, Game& game) override;
};
