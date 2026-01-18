#pragma once

#include "../core/def.h"

#include <deque>


class PlayerBullet {
    Vec2F pos;
    const Renderer& renderer;

public:
    PlayerBullet(Vec2 i_pos): renderer(renderer) {
        pos.x = i_pos.x;
        pos.y = i_pos.y;
    }

    void update() {
        pos.y -= 10;
    }

    void draw(const Renderer* renderer) const {
        renderer->drawSprite(entityTable["playerBullet"],pos);
    }
};


class PlayerBullet_Manager {
    std::deque<PlayerBullet> bullets;

public:
    void generate(const Vec2& pos) {
        bullets.push_back(PlayerBullet(pos));
    }

    void update() {
        if (bullets.size() > 50) bullets.pop_front();
        for (PlayerBullet& bullet: bullets) {
            bullet.update();
        }
    }

    void draw(const Renderer* renderer) const {
        for (const PlayerBullet& bullet: bullets) {
            bullet.draw(renderer);
        }
    }
};
