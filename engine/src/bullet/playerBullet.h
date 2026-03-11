#pragma once

#include "../core/def.h"
#include "../core/entity.h"
#include "../core/collider.h"
#include "../core/entityManager.h"


class PlayerBullet: public EntityBase<PlayerBullet>, ICollidable {
    static constexpr float speed = 800.0f; // pixels per second
    
public:
    PlayerBullet(vec2f i_pos, ColliderHandle col_h, EntityHandle ent_h)
      : EntityBase(ent_h,col_h,i_pos) {}

    bool update(float deltatime, GCMS& gcm) {
        ms.pos.y -= speed * deltatime;
        return true;
    }

    void draw(const Renderer* renderer) const {
        renderer->drawSprite(SpriteID::playerBullet, ms.pos);
    }

    void onHit(const CollisionInfo& info, GCMS& gcm) override {
        // 一旦無視
    }
};


class PlayerBullet_Manager: public EntityManagerBase<PlayerBullet> {
public:
    void generate(const vec2f& pos) {
        EntityHandle e = entMgr.create();
        Collider col{};
        col.type = ColliderType::Circle;
        col.handle = e;
        col.layer = CollisionLayer::playerBullet;
        col.mask = static_cast<uint8_t>(CollisionLayer::enemy);
        col.circle.center = pos;
        col.circle.r = 3.0f;
        auto col_handle = physWorld.add(col);
        
        objects.emplace_back(pos,col_handle,e);
        auto& bullet = objects.back();

        entMgr.setPtr(e,&bullet);
    }

    void update(float deltatime, GCMS& gcm) override {
        if (objects.size() > 50) {
            physWorld.destroy(objects.front().col_h);
            entMgr.destroy(objects.front().ent_h);
            objects.pop_front();
        }
        for (PlayerBullet& bullet: objects) {
            bullet.update(deltatime, gcm);
            physWorld.setPos(bullet.col_h,bullet.ms.pos);
        }
    }
};
