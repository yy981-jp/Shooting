#include "collider.h"

#include <span>
#include <unordered_map>
#include <algorithm>

#include "mathUtil.h"
#include "entityManager.h"


    bool PhysicsWorld::hitCircleCircle(EntityID a, EntityID b) {
        vec2f d = pos[a] - pos[b];
        float rr = radius[a] + radius[b];
        return dot(d,d) < rr*rr;
    }

    bool PhysicsWorld::hitRectCircle(EntityID r, EntityID c) {
        vec2f p = pos[c];
        vec2f mn = aabbMin[r];
        vec2f mx = aabbMax[r];

        vec2f closest{
            std::clamp(p.x, mn.x, mx.x),
            std::clamp(p.y, mn.y, mx.y)
        };

        vec2f d = p - closest;
        return dot(d,d) < radius[c]*radius[c];
    }

    bool PhysicsWorld::hitCircleRect(EntityID c, EntityID r) {
        return hitRectCircle(r, c);
    }

    bool PhysicsWorld::hitRectRect(EntityID a, EntityID b) {
        return aabbOverlap(a, b);
    }

    void PhysicsWorld::updateAABB(EntityID id) {
        if (type[id] == ColliderType::Circle) {
            float r = radius[id];
            vec2f p = pos[id];

            aabbMin[id] = { p.x - r, p.y - r };
            aabbMax[id] = { p.x + r, p.y + r };
        }
        else { // Rect
            aabbMin[id] = rectMin[id];
            aabbMax[id] = rectMax[id];
        }
    }

    bool PhysicsWorld::aabbOverlap(EntityID a, EntityID b) {
        const vec2f& amin = aabbMin[a];
        const vec2f& amax = aabbMax[a];
        const vec2f& bmin = aabbMin[b];
        const vec2f& bmax = aabbMax[b];

        return !(amax.x < bmin.x ||
                amin.x > bmax.x ||
                amax.y < bmin.y ||
                amin.y > bmax.y);
    }

    bool PhysicsWorld::shouldCollide(EntityID a, EntityID b) {
        return (mask[a] & static_cast<uint8_t>(layer[b])) && (mask[b] & static_cast<uint8_t>(layer[a]));
    }

    
    ColliderHandle PhysicsWorld::add(const Collider& c) {
        ColliderID id;

        // ===== free slot reuse =====
        if (freeHead != INVALID) {
            // reuse
            id = freeHead;
            freeHead = records[id].nextFree;
        } else {
            // new slot
            id = (ColliderID)records.size();
            records.push_back({ INVALID, 0 });

            type.push_back({});
            owner.push_back({});
            layer.push_back({});
            mask.push_back({});

            pos.push_back({});
            radius.push_back(0.0f);

            rectMin.push_back({});
            rectMax.push_back({});

            aabbMin.push_back({});
            aabbMax.push_back({});
        }

        // ===== alive 登録（共通）=====
        if (aliveIndex.size() < records.size())
            aliveIndex.resize(records.size());

        aliveIndex[id] = alive.size();
        alive.push_back(id);

        ColliderGen gen = records[id].gen;

        // common
        type[id]  = c.type;
        owner[id] = c.handle;
        layer[id] = c.layer;
        mask[id]  = c.mask;

        // shape reset (important!)
        pos[id] = {};
        radius[id] = 0;
        rectMin[id] = {};
        rectMax[id] = {};

        // shape write
        if (c.type == ColliderType::Circle) {
            pos[id] = c.circle.center;
            radius[id] = c.circle.r;

            aabbMin[id] = c.circle.center - vec2f{c.circle.r, c.circle.r};
            aabbMax[id] = c.circle.center + vec2f{c.circle.r, c.circle.r};
        } 
        else {
            rectMin[id] = c.rect.min;
            rectMax[id] = c.rect.max;

            aabbMin[id] = c.rect.min;
            aabbMax[id] = c.rect.max;
        }

        return { id, gen };
    }

    // 判定ループ（circle only broadphase）
    void PhysicsWorld::step() {
        HitEvents events;

        const size_t N = alive.size();

        // AABB更新
        for (size_t ai = 0; ai < N; ++ai) {
            ColliderID id = alive[ai];
            updateAABB(id);
        }

        // 衝突判定
        for (size_t ai = 0; ai < N; ++ai) {
            ColliderID i = alive[ai];
            for (size_t bi = ai + 1; bi < N; ++bi) {
                ColliderID j = alive[bi];
                if (!shouldCollide(i, j)) continue;
                if (!aabbOverlap(i, j)) continue;

                const auto at = static_cast<size_t>(type[i]);
                const auto bt = static_cast<size_t>(type[j]);

                if ((this->*hitTable[at][bt])(i,j))
                    events.emplace_back(genHitInfo(i,j));
            }
        }

        // 衝突処理
        for (const auto& ev: events) {
            if (auto* a = entMgr.getPtr<ICollidable>(ev.a_handle))
                a->onHit(ev.a_info);
            if (auto* b = entMgr.getPtr<ICollidable>(ev.b_handle))
                b->onHit(ev.b_info);
        }
    }

    void PhysicsWorld::destroy(ColliderHandle h) {
        if (!isAlive(h)) return;

        uint32_t idx  = aliveIndex[h.id];
        ColliderID last = alive.back();

        alive[idx] = last;
        aliveIndex[last] = idx;
        alive.pop_back();

        ++records[h.id].gen;
        records[h.id].nextFree = freeHead;
        freeHead = h.id;
    }
    
    bool PhysicsWorld::isAlive(ColliderHandle h) const {
        return h.id < records.size()
            && records[h.id].gen == h.gen;
    }

    HitEvent PhysicsWorld::genHitInfo(EntityID a, EntityID b) {
        return {
            .a_handle = owner[a],
            .b_handle = owner[b],
            .a_info = CollisionInfo{.layer = layer[b]},
            .b_info = CollisionInfo{.layer = layer[a]}
        };
    }



PhysicsWorld physWorld;
