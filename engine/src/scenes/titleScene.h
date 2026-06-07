#pragma once

#include "../core/def.h"
#include "../core/mathutil.h"
#include "../core/time.h"
#include "../graphics/gfx.h"
#include "scene.h"
#include "ui.h"


struct Widget {
    SpriteID sprite;
    vec2f pos;
    float angle = 0;
    // float scale = 1;
};

enum class wd {
    background,
    // start,
    // end,

    Count
};

class TitleScene: public IScene {
    std::array<Widget, static_cast<size_t>(wd::Count)> widgets;
    uint64_t changed;

    mutable TextUI ui;

public:
    TitleScene(GlobalContext& ctx): IScene(SceneID::title), ui(ctx,{410,-390},{600,390}) {
        widgets[static_cast<size_t>(wd::background)] = {SpriteID::titleBackground, {0,0}};
        // widgets[static_cast<size_t>(wd::start)] = {SpriteID::titleStart, {0, -200}};
        // widgets[static_cast<size_t>(wd::end)] = {SpriteID::titleExit, {0, 200}, 0};

        changed = getUnixTime();
        ctx.sfx->play(SFXID::opening);
    }
    void update(GlobalContext& ctx, const float dt) override {
        if (has(*ctx.key, KCode::z) && (getUnixTime() - changed) >= 2) {
            (*ctx.gcms)(cmd::changeScene{SceneID::play});
            (*ctx.gcms)(cmd::debugMode{false});
        }

        if (has(*ctx.key, KCode::x) && (getUnixTime() - changed) >= 2) {
            (*ctx.gcms)(cmd::changeScene{SceneID::play});
            (*ctx.gcms)(cmd::debugMode{true});
        }
    }
    void draw(const GlobalContext& ctx) const override {
        for (const auto& widget: widgets) {
            ctx.gfx->drawSprite(widget.sprite, widget.pos, widget.angle);
        }
        ctx.gfx->flush();
        drawUi(ctx);
    }

    void drawUi(const GlobalContext& ctx) const {
        ui.initCur();
        ui.write(FontSize::f32, "ScoreBord");
        ui.enter();

        for (const uint64_t& score: ctx.scSys->get()) {
            ui.write(FontSize::f16, score);
            ui.enter();
        }

        ui.write(FontSize::f64, "test");
    }

    // void handleCommand(const GameCommand& cmd, Game& game) {
    //     std::visit(commandExec::Global{game}, cmd);
    // }
};
