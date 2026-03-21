#pragma once

#include "../core/def.h"
#include "../core/collider.h"
#include "../gcms/gcms.h"
#include "../core/entityManager.h"
#include "../core/intervalTrigger.h"
#include "../core/entity.h"
#include "../graphics/gfx.h"
#include "../motion/motion.h"
#include "../tables/all.h"
#include "../motion/motion.h"



struct BezierEnemy: public EntityBase<BezierEnemy>, ICollidable {
    uint16_t hp = 3;
    bool req_enable = false;
    IntervalTrigger spm;
    MotionPipeline mp;
    
    BezierEnemy(const EntityHandle& e, const vec2f& pos,
      std::span<const vec2f> bezierCurve, int duration,
      const ColliderHandle& col_h)
      : EntityBase(e, col_h, pos), spm(1),
        mp(BezierController(bezierCurve,duration,pos)) {}

    bool update(float deltatime, GCMS& gcm) { // false -> 削除,  true -> 生存
        if (!mp.isRunning() || !hp) return false;
        mp.update(deltatime, ms);

        spm.update(deltatime);
        for (int i = 0; i < spm.get(); ++i) {
            for (int rotate = -30; rotate < 30; rotate += 10) {
                cmd::simpleBullet c;
                c.pos = ms.pos;
                c.degree = rotate;
                c.speed = 3;
                gcm(c);
            }
        }

        return true;
    }

    void draw(const Renderer* renderer) const {
        renderer->drawSprite(SpriteID::bezierEnemy, ms.pos, ms.angle);
    }
    
    void onHit(const CollisionInfo& info, GCMS& gcm) override {
        if (info.layer == CollisionLayer::playerBullet) {
            // 点生成
            cmd::pointBullet c;
            c.pos = ms.pos;
            gcm(c);
            // 衰弱
            applyDamage(hp, physWorld.getPtr(info.col_h).pb->attackPower);
            // 直接加点
            gcm(cmd::addScore{10});
        }
    }
};



class BezierEnemy_Manager: public EntityManagerBase<BezierEnemy> {
    struct BMCache {
        std::string_view get(const int BezierCurveType) const {
            auto it = table.find(BezierCurveType);
            if (it == table.end()) 
                throw std::runtime_error("BezierEnemy_Manager::CacheSV: not found - id: "
                    + std::to_string(BezierCurveType));
            return it->second;
        }
        std::unordered_map<uint16_t, std::string> table; // 整数型がkeyなのでrapidString系は使わない
    } bmcache;

public:
    BezierEnemy_Manager() {
        auto arr = paramTable.json["param"]["bezierEnemy"]["patterns"].GetArray();
        int index = 0;
        for (const auto& v: arr) {
            bmcache.table[index++] = v.GetString();
        }
    }

    void generate(const vec2f& pos, const int BezierCurveType, const int duration) {
        std::string_view curveAlias = bmcache.get(BezierCurveType);
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
        col.circle.r = 5.f; // 敵の当たり判定半径

        // ===== Physics登録 =====
        auto col_handle = physWorld.add(col);

        // ===== bezierEnemy生成 =====
        objects.emplace_back(e, pos, controlVec2, duration, col_handle);
        BezierEnemy& enemy = objects.back();

        // EntityManagerにポインタ登録
        physWorld.setColPtr(col_handle,&enemy);
    }
};
