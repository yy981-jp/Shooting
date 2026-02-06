#include "def.h"
#include "entityManager.h"
#include "../tables/entityTable.h"

#include <variant>
#include <vector>
#include <span>
#include <concepts>
#include <array>
#include <unordered_map>


enum class CollisionLayer: uint8_t {
    player,
    playerBullet,
    enemy,
    enemyBullet,

    max
};

struct CollisionInfo {
    EntityHandle handle;
};

struct ICollidable {
	virtual void onHit(const CollisionInfo& info) = 0;
};


enum class ColliderType : uint8_t {
	Circle,
	Rect
};

struct Collider {
	ColliderType type;
    EntityHandle handle;

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

struct HitEvent {
    EntityHandle a_handle, b_handle;
    CollisionInfo a_info, b_info;
};

using HitEvents = std::vector<HitEvent>;

// 2つのCollisionLayer同士を見て衝突判定
class CollisionManager {
    static bool hitCircleCircle(const Collider& a, const Collider& b) {
        // TODO: 円同士の判定
        return false;
    }

    static bool hitCircleRect(const Collider& a, const Collider& b) {
        // TODO: 円 vs 矩形
        return false;
    }

    static bool hitRectRect(const Collider& a, const Collider& b) {
        // TODO: 矩形同士
        return false;
    }

    // Rect vs Circle は Circle vs Rect を使い回す
    static bool hitRectCircle(const Collider& a, const Collider& b) {
        return hitCircleRect(b, a);
    }

    using HitFunc = bool(*)(const Collider&, const Collider&);

    static constexpr HitFunc hitTable[2][2] = {
        // a = Circle
        { hitCircleCircle, hitCircleRect },
        // a = Rect
        { hitRectCircle,   hitRectRect }
    };

public:
    bool hit(const Collider& a, const Collider& b) {
        const auto at = static_cast<size_t>(a.type);
        const auto bt = static_cast<size_t>(b.type);

        return hitTable[at][bt](a, b);
    }

    HitEvents check(std::span<std::span<Collider>> a, std::span<std::span<Collider>> b) {
        HitEvents ev;
        for (const auto& colliders_a: a) for (const auto& colliders_b: b)
        for (const auto& collider_a: colliders_a) {
            for (const auto& collider_b: colliders_b) {
                // ここにAABBを使った大雑把な事前衝突判定処理を置きたい
                if (hit(collider_a,collider_b)) {
                    HitEvent h_ev{
                        .a_handle = collider_a.handle,
                        .b_handle = collider_b.handle,
                        .a_info = CollisionInfo{collider_b.handle},
                        .b_info = CollisionInfo{collider_a.handle}
                    };
                    ev.emplace_back(h_ev);
                }
            }
        }
        return ev;
    }
};


// データの流れを制御
class CollisionSystem {
    CollisionManager cm;

    std::array<std::vector<std::span<Collider>>,static_cast<size_t>(CollisionLayer::max)> layers;

    void set(std::span<Collider> colliders, CollisionLayer layer) {
        layers[static_cast<size_t>(layer)].emplace_back(colliders);
    }

    void check() {
        std::array<HitEvents,3/*衝突組み合わせ数*/> res;

        res[0] = cm.check(
            layers[static_cast<size_t>(CollisionLayer::enemy)],
            layers[static_cast<size_t>(CollisionLayer::playerBullet)]
        );
        res[1] = cm.check(
            layers[static_cast<size_t>(CollisionLayer::enemy)],
            layers[static_cast<size_t>(CollisionLayer::player)]
        );
        res[2] = cm.check(
            layers[static_cast<size_t>(CollisionLayer::enemyBullet)],
            layers[static_cast<size_t>(CollisionLayer::player)]
        );

        // 衝突処理実行
        for (const auto& res_c: res) for (const auto& ev: res_c) {
            if (auto* a_ptr = entMgr.getPtr<ICollidable>(ev.a_handle)) a_ptr->onHit(ev.a_info);
            if (auto* b_ptr = entMgr.getPtr<ICollidable>(ev.b_handle)) b_ptr->onHit(ev.a_info);
        }

        for (auto& layer: layers) layer.clear(); // layers初期化
    }
};
