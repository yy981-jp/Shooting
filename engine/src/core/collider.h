#pragma once

#include "def.h"
#include "hitEvent.h"
#include "../graphics/gfx.h"
#include "../gcms/gcms.h"


// 当たり判定があるentityはこれを継承する
struct ICollidable {
	virtual void onHit(const CollisionInfo& info) = 0;
};

// 下の構造体のためのenumclass
enum class ColliderType : uint8_t {
	Circle,
	Rect
};
// 当たり判定の形状とEntityHandle
struct Collider {
	ColliderType type;
    EntityHandle handle;
    CollisionLayer layer; // 自分の所属
    uint8_t mask;         // 当たりたい相手

	union {
		struct {
			vec2f center;
			float r;
		} circle;

		struct {
			vec2f min;
			vec2f max;
		} rect;
	};
};

// 基本情報
using ColliderID = uint32_t;
using ColliderGen = uint32_t;
struct ColliderHandle {
    ColliderID id;
    ColliderGen gen;
};


class PhysicsWorld {
public:
    struct Entry {
        uint32_t nextFree;
        EntityGen gen;
    };

private:
    static constexpr ColliderID INVALID = UINT32_MAX;

    // SoA（データを分離）
    // 共通
    std::vector<ColliderType> type;
    std::vector<vec2f> pos;        // center (circle) / center (rect)
    std::vector<CollisionLayer> layer;
    std::vector<uint8_t> mask;

    // Circle専用
    std::vector<float> radius;

    // Rect専用
    std::vector<vec2f> rectMin, rectMax;

    // AABB (broadcheck用 大雑把な当たり判定)
    std::vector<vec2f> aabbMin, aabbMax;
    
    std::vector<EntityHandle> owner;

    std::vector<ColliderID> alive;
    std::vector<uint32_t> aliveIndex;
    std::vector<Entry> records;
    ColliderID freeHead = INVALID;


    bool hitCircleCircle(EntityID a, EntityID b);
    bool hitRectCircle(EntityID r, EntityID c);
    bool hitCircleRect(EntityID c, EntityID r);
    bool hitRectRect(EntityID a, EntityID b);
    using HitFunc = bool (PhysicsWorld::*)(EntityID, EntityID);
    HitFunc hitTable[2][2] = {
        { &PhysicsWorld::hitCircleCircle, &PhysicsWorld::hitCircleRect },
        { &PhysicsWorld::hitRectCircle,   &PhysicsWorld::hitRectRect }
    };

    void updateAABB(EntityID id);
    bool aabbOverlap(EntityID a, EntityID b);
    bool shouldCollide(EntityID a, EntityID b);
    HitEvent genHitInfo(EntityID a, EntityID b);

public:
    void draw(const Renderer* r) {
        r->setColor(255,255,255,255);
        for (const auto& e: alive) {
            r->drawFilledCircle(pos[e], radius[e]);
        }
    }

    ColliderHandle add(const Collider& c);

    // 判定ループ（circle only broadphase）
    void step(GCMS& gcm);

    void setPos(ColliderHandle h, vec2f p) {
        pos[h.id] = p;
    }

    void destroy(ColliderHandle h);
    bool isAlive(ColliderHandle h) const;
};

extern PhysicsWorld physWorld;
