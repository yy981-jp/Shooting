#pragma once

#include "../core/def.h"
#include "scene.h"


struct TitleScene: public IScene {
    TitleScene(SceneContext& ctx) {};
    void update(SceneContext& ctx, const float dt) override {};
    void draw(const SceneContext& ctx) const override {
        ctx.gfx->drawSpriteNow(EntityType::titleBackground, {0,0});
    };
    void handleCommand(const GameCommand& cmd, Game& game) override {};
};
