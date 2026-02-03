#include "def.h"
#include "../tables/entityTable.h"

#include <variant>
#include <vector>
#include <span>
#include <concepts>


enum class CollisionLayer: uint8_t {
    player,
    playerBullet,
    enemy,
    enemyBullet
};

struct Collider_base {
    CollisionLayer layer;
};

struct Circle: public Collider_base {
	vec2f center;
	float r;
};

struct Rect: public Collider_base {
	vec2f min;
	vec2f max;
};


using Collider = std::variant<Circle, Rect>;

struct Colliders {
    CollisionLayer layer;
    std::span<Collider> data;
}

template<typename T>
concept HasOnHit = requires(T t) {
	t.onHit();
};

struct HitEvent {
	// int a, b; <- intではなくentityHandleを作りそれを使用
};

struct HitEvent_vec {
    EntityType type;
    std::vector<EntityHandle> event;
};

using HitEvents = std::vector<HitEvent_vec>;

class CollisionManager {
    template<class A, class B>
    HitEvents check(std::span<A> a, std::span<B> b) {
        for (auto& x : a) {
            for (auto& y : b) {
                if (hit(x.getCollider(), y.getCollider())) {
                    onHit(x, y);
                }
            }
        }
    }
};

class CollisionSystem {
    CollisionManager cm;

    // using enum CollisionLayer;

    std::vector<std::span<Collider>> list;

    void set(std::span<Collider> a) {
        
    }

    void check() {
        for (auto entities: list) {
            for (auto entity: entities) {
                entity.
            }
        }
        HitEvents events = cm.check();
        
        // parse events
        A a_ptr = entityTable.pointerTable.at(events.a.type);
    }

    void divi(HitEvents) {

    }
};