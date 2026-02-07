#pragma once

#include "def.h"

#include <vector>


// 衝突層
enum class CollisionLayer: uint8_t {
    wall            = 1 << 0,
    player          = 1 << 1,
    playerBullet    = 1 << 2,
    enemy           = 1 << 3,
    enemyBullet     = 1 << 4
};

// CollisionSystemが衝突処理関数に情報を渡すのに使う構造体
struct CollisionInfo {
    EntityHandle handle;
};

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

// 衝突に関する情報全部
struct HitEvent {
    EntityHandle a_handle, b_handle;
    CollisionInfo a_info, b_info;
};
using HitEvents = std::vector<HitEvent>;


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
    ColliderHandle add(const Collider& c);

    // 判定ループ（circle only broadphase）
    void step();

    void destroy(ColliderHandle h);
    bool isAlive(ColliderHandle h) const;
};

extern PhysicsWorld physWorld;
