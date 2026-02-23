#pragma once

#include "../core/def.h"
#include "../core/collider.h"
#include "../core/commands.h"
#include "../core/entityManager.h"
#include "../core/spawnManager.h"
#include "../graphics/gfx.h"
#include "../controller/all.h"
#include "../tables/all.h"
#include "../controller/all.h"


struct EnemyBezier: public ICollidable {
    EntityHandle ent;
    ColliderHandle col_h;
    vec2f spriteHalf;
    bool wasShot = false;
    bool req_enable = false;
    spawnManager spm;

    MotionState ms;
    MotionPipeline mp;
    
    EnemyBezier(const EntityHandle& e, const MotionState& i_ms,
      std::span<const vec2f> bezierCurve, int duration,
      const ColliderHandle& col_h, const vec2f& spriteHalf)
      : ms(i_ms), mp(BezierController(bezierCurve,duration,i_ms.pos)),
        ent(e), col_h(col_h), spriteHalf(spriteHalf), spm(300) {
            // WaveDecorator WaveDecorator(wave_amp,wave_freq);
            // mp.addMover(WaveDecorator);
        }

    bool update(float deltatime, GCMS& gcm) { // true -> 有効,  false -> 削除
        if (!mp.isRunning() || wasShot) return false;
        mp.update(deltatime, ms);

        spm.update(deltatime);
        for (int i = 0; i < spm.get(); ++i) {
            for (int rotate = 0; rotate < 360; rotate += 10) {
                cmd::simpleBullet c;
                c.pos = ms.pos;
                c.rotate = rotate;
                c.speed = 3;
                gcm(c);
            }
        }

        return true;
    }

    void draw(const Renderer* renderer) const {
        renderer->drawSprite(EntityType::enemyBezier, ms.pos-spriteHalf);
    }

    void onHit(const CollisionInfo& info) {
        if (info.layer == CollisionLayer::playerBullet) wasShot = true;
    }
};



class EnemyBezier_Manager {
    std::deque<EnemyBezier> list;
    vec2f spriteHalf;

    struct BMCache {
        std::string_view get(const int BezierCurveType) const {
            auto it = table.find(BezierCurveType);
            if (it == table.end()) 
                throw std::runtime_error("enemyBezier_Manager::CacheSV: not found - id: "
                    + std::to_string(BezierCurveType));
            return it->second;
        }
        std::unordered_map<uint16_t, std::string> table; // 整数型がkeyなのでrapidString系は使わない
    } bmcache;

public:
    EnemyBezier_Manager(const vec2f& spriteHalf): spriteHalf(spriteHalf) {
        auto arr = paramTable.json["param"]["enemyBezier"]["patterns"].GetArray();
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
        col.circle.r = 5.0f; // 敵の当たり判定半径（仮）

        // ===== Physics登録 =====
        auto col_handle = physWorld.add(col);

        // ===== EnemyBezier生成 =====
        list.emplace_back(e, MotionState{pos}, controlVec2, duration, col_handle, spriteHalf);
        EnemyBezier& enemy = list.back();

        // EntityManagerにポインタ登録（OOP方式）
        entMgr.setPtr(e,&enemy);
    }

    void update(float deltatime, GCMS& gcm) {
        for (size_t i = 0; i < list.size(); ) {
            if (!list[i].update(deltatime,gcm)) { // 削除されていた場合
                physWorld.destroy(list[i].col_h);
                entMgr.destroy(list[i].ent);
                list[i] = std::move(list.back());
                entMgr.setPtr(list[i].ent, &list[i]); // moveされたentityのptrを更新
                list.pop_back();
            } else {
                physWorld.setPos(list[i].col_h,list[i].ms.pos);
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