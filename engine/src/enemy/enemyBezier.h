#pragma once

#include "../core/def.h"
#include "../core/collider.h"
#include "../core/entityManager.h"
#include "../graphics/gfx.h"
#include "BezierMover.h"
#include "../tables/all.h"


struct EnemyBezier: public ICollidable {
    vec2f pos;
    vec2f origin;
    vec2i border;
    BezierMover bm;
    EntityHandle ent;
    ColliderHandle col_h;
    vec2f spriteHalf;
    bool wasShot = false;
    
    EnemyBezier(const EntityHandle& e, const vec2f& i_pos,
      std::span<const vec2f> bezierCurve, int duration, const vec2i& border,
      const ColliderHandle& col_h, const vec2f& spriteHalf)
      : pos(i_pos), origin(pos - bezierCurve[0]), border(border), bm(bezierCurve,duration), ent(e), col_h(col_h), spriteHalf(spriteHalf) {}

    bool update(int deltatime) { // true -> 有効,  false -> 削除
        if (!bm.isRunning() || wasShot) return false;
        bm.update(deltatime);
        pos = bm.pos + origin;
        return true;
    }

    void draw(const Renderer* renderer) const {
        renderer->drawSprite(entityTable.get("enemyBezier"),pos-spriteHalf);
    }

    void onHit(const CollisionInfo& info) {
        if (info.layer == CollisionLayer::playerBullet) wasShot = true;
    }
};



class EnemyBezier_Manager {
    std::deque<EnemyBezier> list;
    const vec2i& border;
    const EntityType type;
    vec2f spriteHalf;

    struct Cache {
        std::string_view get(const int BezierCurveType) const {
            auto it = table.find(BezierCurveType);
            if (it == table.end()) 
                throw std::runtime_error("enemyBezier_Manager::Cache: not found - id: "
                    + std::to_string(BezierCurveType));
            return it->second;
        }
        std::unordered_map<uint16_t, std::string> table; // 整数型がkeyなのでrapidString系は使わない
    } cache;

public:
    EnemyBezier_Manager(const Renderer* r, const vec2i& i_border): border(i_border), type(entityTable.get("enemyBezier")) {
        auto arr = paramTable.json["param"]["enemyBezier"]["patterns"].GetArray();
        int index = 0;
        for (const auto& v: arr) {
            cache.table[index++] = v.GetString();
        }

        spriteHalf = r->getSpriteSize(type) / 2;
    }

    void generate(const vec2f& pos, const int BezierCurveType, const int duration) {
        std::string_view curveAlias = cache.get(BezierCurveType);
        auto controlVec2 = paramTable.bezierCurve.get(curveAlias);

        // ===== Entity生成 =====
        EntityHandle e = entMgr.create();

        // ===== Collider作成 =====
        Collider col{};
        col.type   = ColliderType::Circle;
        col.handle = e;
        col.layer  = CollisionLayer::enemy;
        col.mask   =
            static_cast<uint8_t>(CollisionLayer::player) |
            static_cast<uint8_t>(CollisionLayer::playerBullet);

        col.circle.center = pos;
        col.circle.r = 5.0f; // 敵の当たり判定半径（仮）

        // ===== Physics登録 =====
        auto col_handle = physWorld.add(col);

        // ===== EnemyBezier生成 =====
        list.emplace_back(e, pos, controlVec2, duration, border, col_handle, spriteHalf);
        EnemyBezier& enemy = list.back();

        // EntityManagerにポインタ登録（OOP方式）
        entMgr.setPtr(e,&enemy);
    }

    void update(int deltatime) {
        for (size_t i = 0; i < list.size(); ) {
            if (!list[i].update(deltatime)) { // 削除されていた場合
                physWorld.destroy(list[i].col_h);
                list[i] = std::move(list.back());
                list.pop_back();
            } else {
                physWorld.setPos(list[i].col_h,list[i].pos);
                ++i;
            }
        }
    }

    void draw(const Renderer* renderer) const {
        for (const auto& bullet: list) {
            bullet.draw(renderer);
        }
    }
};