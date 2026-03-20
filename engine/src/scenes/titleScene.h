#pragma once

#include "../core/def.h"
#include "../core/mathUtil.h"
#include "../graphics/gfx.h"
#include "scene.h"


struct Widget {
    SpriteID sprite;
    vec2f pos;
    float angle = 0;
    // float scale = 1;
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
    TitleScene(GlobalContext& ctx): IScene(SceneID::title) {
        widgets[static_cast<size_t>(wd::background)] = {SpriteID::titleBackground, {0,0}};
        widgets[static_cast<size_t>(wd::start)] = {SpriteID::titleStart, {0, -200}};
        widgets[static_cast<size_t>(wd::end)] = {SpriteID::titleExit, {0, 200}, 0};

        ctx.sfx->play(SFXID::opening);
    }
    void update(GlobalContext& ctx, const float dt) override {
        // DEBUG
        if (has(*ctx.key, KCode::z)) {
            (*ctx.gcms)(cmd::changeScene{SceneID::play});
        }
    }
    void draw(const GlobalContext& ctx) const override {
        for (const auto& widget: widgets) {
            ctx.gfx->drawSprite(widget.sprite, widget.pos, widget.angle);
        }
        ctx.gfx->flush();
    }
    // void handleCommand(const GameCommand& cmd, Game& game) {
    //     std::visit(commandExec::Global{game}, cmd);
    // }
};
