#pragma once

#include "../core/def.h"
#include "../core/mathUtil.h"
#include "../graphics/gfx.h"
#include "scene.h"


struct Widget {
    SpriteID sprite;
    vec2f pos;
    float angle = 0;
    float scale = 1;
};

enum class wd {
    background,
    start,
    end,

    Count
};

class TitleScene: public IScene {
    std::array<Widget, static_cast<size_t>(wd::Count)> widgets;

public:
    TitleScene(SceneContext& ctx) {
        widgets[static_cast<size_t>(wd::background)] = {SpriteID::titleBackground, {0,0}};
        widgets[static_cast<size_t>(wd::start)] = {SpriteID::titleStart, {0, -200}};
        widgets[static_cast<size_t>(wd::end)] = {SpriteID::titleExit, {0, 200}, 0, 1.2};

        ctx.sfx->play(SFXID::opening);
    };
    void update(SceneContext& ctx, const float dt) override {};
    void draw(const SceneContext& ctx) const override {
        for (const auto& widget: widgets) {
            ctx.gfx->drawSpriteNow(widget.sprite, widget.pos, widget.angle, widget.scale);
        }
    };
    void handleCommand(const GameCommand& cmd, Game& game) override {};
};
