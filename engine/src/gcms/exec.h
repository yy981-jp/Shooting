#pragma once
#include "gcms.h"
#include "../core/Game.h"
#include "../core/entityManager.h"
#include "../core/collider.h"

namespace commandExec {

struct base {
    Game& game;

    void operator()(const cmd::sfx& c) const { game.playSfx(c.id); }
    void operator()(const cmd::changeScene& c) const { game.setScene(c.id); }
    void operator()(const cmd::onHit& c) const {
        // 衝突処理
        for (const auto& ev: c.events) {
            if (auto* a = entMgr.getPtr<ICollidable>(ev.a_handle))
                a->onHit(ev.a_info, game.gcm);
            if (auto* b = entMgr.getPtr<ICollidable>(ev.b_handle))
                b->onHit(ev.b_info, game.gcm);
        }
    }
};

}
