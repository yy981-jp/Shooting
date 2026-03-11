#include "../core/def.h"
#include "../core/cache.h"
#include "../core/entity.h"
#include "../core/entityManager.h"
#include "../core/collider.h"
#include "../core/util.h"
#include "../core/mathutil.h"

#include <deque>


struct PointBullet: public EntityBase<PointBullet>, ICollidable {
    vec2f spriteHalf;
    bool alive = true;
    static constexpr float speed = 150.f;

    PointBullet(vec2f pos, float angle, EntityHandle eh, ColliderHandle ch, const vec2f& spriteHalf)
      : EntityBase(eh, ch, pos), spriteHalf(spriteHalf) {}

    bool update(float dt, GCMS& gcm) {
        if (!alive) return false;
        ms.pos.y += speed * dt;
        if (isOffScreen(ms.pos,spriteHalf)) alive = false;
        return true;
    }

    void draw(const Renderer* r) const {
        r->drawSprite(SpriteID::pointBullet, ms.pos);
    }

    void onHit(const CollisionInfo& info, GCMS& gcm) override {
        alive = false;
    }
};

class PointBullet_Manager: public EntityManagerBase<PointBullet> {
    vec2f spriteHalf;

public:
    PointBullet_Manager(const vec2f& spriteHalf): spriteHalf(spriteHalf) {}

    void generate(const vec2f& pos, bool homingMode) {
        EntityHandle eh = entMgr.create();

        Collider col{};
        col.type   = ColliderType::Circle;
        col.handle = eh;
        col.layer  = CollisionLayer::item;
        col.mask   = static_cast<uint8_t>(CollisionLayer::player);

        col.circle.center = pos;
        col.circle.r = 10.0f;

        ColliderHandle ch = physWorld.add(col);

        float angle = (homingMode? CacheSV::OFFSET+deg2rad(180) : 0);
        
        objects.emplace_back(pos, angle, eh, ch, spriteHalf);

        entMgr.setPtr(eh, &objects.back());
    }
};
