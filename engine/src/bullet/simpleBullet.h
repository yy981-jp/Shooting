#include "../core/def.h"
#include "../core/cache.h"
#include "../core/entity.h"
#include "../core/entityManager.h"
#include "../core/collider.h"
#include "../core/util.h"
#include "../core/mathutil.h"
#include "../motion/motion.h"

#include <deque>


struct SimpleBullet: public EntityBase<SimpleBullet>, ICollidable {
    vec2f spriteHalf;
    bool alive = true;

    MotionPipeline mp;

    SimpleBullet(vec2f pos, float angle, float speed, EntityHandle eh, ColliderHandle ch, const vec2f& spriteHalf)
      : EntityBase(eh, ch, pos), spriteHalf(spriteHalf), mp(LineController(angle,speed)) {}

    bool update(float dt, GCMS& gcm) {
        if (!alive) return false;
        ms.pos += mp.update(dt,ms);
        if (isOffScreen(ms.pos,spriteHalf)) alive = false;
        return true;
    }

    void draw(const Renderer* r) const {
        r->drawSprite(SpriteID::simpleBullet, ms.pos, ms.angle);
    }
};

class SimpleBullet_Manager: public EntityManagerBase<SimpleBullet> {
    vec2f spriteHalf;

public:
    SimpleBullet_Manager(const vec2f& spriteHalf): spriteHalf(spriteHalf) {}

    void generate(const vec2f& pos, int degree, float speed) {
        EntityHandle eh = entMgr.create();

        Collider col{};
        col.type   = ColliderType::Circle;
        col.handle = eh;
        col.layer  = CollisionLayer::enemyBullet;
        col.mask   = static_cast<uint8_t>(CollisionLayer::player);

        col.circle.center = pos;
        col.circle.r = 3.0f;

        ColliderHandle ch = physWorld.add(col);
        
        objects.emplace_back(pos, deg2rad(degree), speed, eh, ch, spriteHalf);

        entMgr.setPtr(eh, &objects.back());
    }
};
