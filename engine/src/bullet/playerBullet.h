#pragma once

#include "../core/def.h"
#include "../core/collider.h"
#include "../core/entityManager.h"

#include <deque>


class PlayerBullet: public ICollidable {
    const Renderer& renderer;
    static constexpr float speed = 600.0f; // pixels per second
    
public:
    vec2f pos;
    ColliderHandle col_h; 

    PlayerBullet(vec2i i_pos, ColliderHandle col_h): renderer(renderer), col_h(col_h) {
        pos.x = i_pos.x;
        pos.y = i_pos.y;
    }

    void update(int deltaTime) {
        pos.y -= speed * (static_cast<float>(deltaTime) / 1000.0f);
    }

    void draw(const Renderer* renderer) const {
        renderer->drawSprite(entityTable.get("playerBullet"),pos);
    }

    void onHit(const CollisionInfo& info) {
        // 一旦無視
    }
};


class PlayerBullet_Manager {
    std::deque<PlayerBullet> bullets;

public:
    void generate(const vec2i& pos) {
        EntityHandle e = entMgr.create();
        Collider col{};
        col.type = ColliderType::Circle;
        col.handle = e;
        col.layer = CollisionLayer::playerBullet;
        col.mask = static_cast<uint8_t>(CollisionLayer::enemy);
        col.circle.center = pos;
        col.circle.r = 12.0f;
        auto col_handle = physWorld.add(col);
        
        bullets.emplace_back(pos,col_handle);
        auto& bullet = bullets.back();

        entMgr.setPtr(e,&bullet);
    }

    void update(int deltaTime) {
        if (bullets.size() > 50) {
            physWorld.destroy(bullets.front().col_h);
            bullets.pop_front();
        }
        for (PlayerBullet& bullet: bullets) {
            bullet.update(deltaTime);
            physWorld.setPos(bullet.col_h,bullet.pos);
        }
    }

    void draw(const Renderer* renderer) const {
        for (const PlayerBullet& bullet: bullets) {
            bullet.draw(renderer);
        }
    }
};
