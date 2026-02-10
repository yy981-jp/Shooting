#include "../core/cache.h"
#include "../core/EntityManager.h"
#include "../core/collider.h"
#include "../core/def.h"

#include <deque>


struct SimpleBullet: public ICollidable {
    vec2f pos, vel;
    vec2f spriteHalf;
    EntityHandle eh;
    ColliderHandle ch;
    bool alive = true;

    SimpleBullet(vec2f pos, float speed, vec2f dir, EntityHandle eh, ColliderHandle ch, const vec2f& spriteHalf)
      : pos(pos), eh(eh), ch(ch), spriteHalf(spriteHalf) {
        vel = dir * speed;
    }

    bool update() {
        if (!alive) return false;
        pos += vel;
        return true;
    }

    void draw(const Renderer* r) const {
        r->drawSprite(entityTable.get("simpleBullet"),pos-spriteHalf);
    }

    void onHit(const CollisionInfo& info) override {
        // 流す
    }
};

class SimpleBullet_Manager {
    const Cache& cache;
    std::deque<SimpleBullet> list;
    vec2f spriteHalf;

public:
    SimpleBullet_Manager(const Renderer* r, const Cache& cache): cache(cache), 
      spriteHalf(r->getSpriteSize(entityTable.get("simpleBullet"))) {}

    void generate(const vec2f& pos, int rotate, float speed) {
        EntityHandle eh = entMgr.create();

        Collider col{};
        col.type   = ColliderType::Circle;
        col.handle = eh;
        col.layer  = CollisionLayer::enemyBullet;
        col.mask   = static_cast<uint8_t>(CollisionLayer::player);

        col.circle.center = pos;
        col.circle.r = 3.0f;

        ColliderHandle ch = physWorld.add(col);
        vec2f dir = cache.getDir(rotate);
        
        list.emplace_back(pos, speed, dir, eh, ch, spriteHalf);

        entMgr.setPtr(eh, &list.back());
    }

    void update(int deltatime) {
        for (size_t i = 0; i < list.size(); ) {
            if (!list[i].update()) { // 削除されていた場合
                physWorld.destroy(list[i].ch);
                entMgr.destroy(list[i].eh);
                list[i] = std::move(list.back());
                entMgr.setPtr(list[i].eh, &list[i]); // moveされたentityのptrを更新
                list.pop_back();
            } else {
                physWorld.setPos(list[i].ch,list[i].pos);
                ++i;
            }
        }
    }

    void draw(const Renderer* r) {
        for (const auto& entity: list) {
            entity.draw(r);
        }
    }
};
