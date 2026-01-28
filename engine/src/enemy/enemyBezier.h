#include "../core/def.h"
#include "../graphics/gfx.h"
#include "BezierMover.h"
#include "ParamTable.h"

#include <deque>

class EnemyBezier {
    Vec2F pos;
    const Vec2& border;
    Vec2 d;
    BezierMover* bm;

public:
    EnemyBezier(const Vec2& i_pos, const int& BezierCurveTime, const int& duration, const Vec2& border)
     : pos(i_pos), border(border) {
        std::vector<Vec2F> controlVec2 = paramTable.bezierCurve.get();
        bm = new BezierMover(controlVec2,duration); // init BezierMover
    }

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

public:
    enemyBezier_Manager(const Vec2& i_border): border(i_border) {}

    void generate(const Vec2& pos) {
        list.push_back(EnemyBezier(pos,0,0,border)); // borderは定数
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