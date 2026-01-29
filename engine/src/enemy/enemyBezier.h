#include "../core/def.h"
#include "../graphics/gfx.h"
#include "BezierMover.h"
#include "ParamTable.h"

#include <deque>


class EnemyBezier {
    Vec2F pos;
    const Vec2& border;
    Vec2 d;
    BezierMover bm;

public:
    EnemyBezier(const Vec2& i_pos, std::span<const Vec2F> BezierCurve, const int duration, const Vec2& border)
     : pos(i_pos), border(border), bm(BezierCurve,duration) {}

    bool update() { // true -> 有効,  false -> 削除
        pos.x += d.x;
        pos.y += d.y;
        if (pos.y <= 0 || pos.y >= border.y || pos.x <= 0 || pos.x <= border.x)
            return false; else return true;
    }

    void draw(const Renderer* renderer) const {
        renderer->drawSprite(entityTable.get("enemyBezier"),pos);
    }
};



class enemyBezier_Manager {
    std::deque<EnemyBezier> list;
    const Vec2& border;

    struct Cache {
        std::string_view get(const int BezierCurveType) const {
            auto it = table.find(BezierCurveType);
            if (it == table.end()) throw std::runtime_error("enemyBezier_Manager::Cache: not found");
            return it->second;
        }
        std::unordered_map<uint16_t, std::string> table; // 整数型がkeyなのでrapidString系は使わない
    } cache;

public:
    enemyBezier_Manager(const Vec2& i_border): border(i_border) {}

    void generate(const Vec2& pos, const int BezierCurveType, const int duration) {
        std::string_view curveAlias = cache.get(BezierCurveType);
        auto controlVec2 = paramTable.bezierCurve.get(curveAlias);
        list.push_back(EnemyBezier(pos,controlVec2,0,border)); // borderは定数
    }

    void update() {
        if (list.size() > 50) list.pop_front();
        for (auto& enemy: list) {
            enemy.update();
        }
    }

    void draw(const Renderer* renderer) const {
        for (const auto& bullet: list) {
            bullet.draw(renderer);
        }
    }
};