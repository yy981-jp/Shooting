#pragma once

#include "../VM/VM.h"
#include "../player/player.h"
#include "../bullet/playerBullet.h"
#include "../bullet/simpleBullet.h"
#include "../bullet/pointBullet.h"
#include "../enemy/bezierEnemy.h"

#include "scene.h"
#include "playSceneUI.h"



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
    std::string buildID, buildTimeStamp;

    struct FrameState {
        vec2f playerPos;
    } frameState;

    mutable PlaySceneUI ui;
    float currentFps;

    VM vm;

    Player player;
    PlayerBullet_Manager playerBullet_Manager;
    BezierEnemy_Manager bezierEnemy_Manager;
    SimpleBullet_Manager simpleBullet_Manager;
    PointBullet_Manager pointBullet_Manager;

    PlayScene(SceneContext& ctx);
    void update(SceneContext& ctx, const float dt) override;
    void draw(const SceneContext& ctx) const override;
    void drawUI(const SceneContext& ctx) const;
    void handleCommand(const GameCommand& cmd, Game& game) override;
};
