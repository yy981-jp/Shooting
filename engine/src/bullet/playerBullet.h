#pragma once

#include "../core/def.h"

#include <deque>


class PlayerBullet {
    vec2f pos;
    const Renderer& renderer;
    static constexpr float speed = 600.0f; // pixels per second

public:
    PlayerBullet(vec2i i_pos): renderer(renderer) {
        pos.x = i_pos.x;
        pos.y = i_pos.y;
    }

    void update(int deltaTime) {
        pos.y -= speed * (static_cast<float>(deltaTime) / 1000.0f);
    }

    void draw(const Renderer* renderer) const {
        renderer->drawSprite(entityTable.get("playerBullet"),pos);
    }
};


class PlayerBullet_Manager {
    std::deque<PlayerBullet> bullets;

public:
    void generate(const vec2i& pos) {
        bullets.push_back(PlayerBullet(pos));
    }

    void update(int deltaTime) {
        if (bullets.size() > 50) bullets.pop_front();
        for (PlayerBullet& bullet: bullets) {
            bullet.update(deltaTime);
        }
    }

    void draw(const Renderer* renderer) const {
        for (const PlayerBullet& bullet: bullets) {
            bullet.draw(renderer);
        }
    }
};
