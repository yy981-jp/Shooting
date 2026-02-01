#pragma once

#include "../core/def.h"
#include "../graphics/gfx.h"
#include "BezierMover.h"
#include "../tables/all.h"


class EnemyBezier {
    vec2f pos, origin;
    vec2i border;
    BezierMover bm;

public:
    EnemyBezier(const vec2i& i_pos, std::span<const vec2f> BezierCurve, const int duration, const vec2i& border)
     : pos(i_pos), origin(i_pos), border(border), bm(BezierCurve,duration) {}

    bool update(int deltatime) { // true -> 有効,  false -> 削除
        if (!bm.isRunning()) return false;
        bm.update(deltatime);
        pos = bm.pos + origin;
        return true;
    }

    void draw(const Renderer* renderer) const {
        renderer->drawSprite(entityTable.get("enemyBezier"),pos);
    }
};



class EnemyBezier_Manager {
    std::vector<EnemyBezier> list;
    const vec2i& border;

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
    EnemyBezier_Manager(const vec2i& i_border): border(i_border) {
        auto arr = paramTable.json["param"]["enemyBezier"]["patterns"].GetArray();
        int index = 0;
        for (const auto& v: arr) {
            cache.table[index++] = v.GetString();
        }
    }

    void generate(const vec2i& pos, const int BezierCurveType, const int duration) {
        std::string_view curveAlias = cache.get(BezierCurveType);
        auto controlVec2 = paramTable.bezierCurve.get(curveAlias);
        list.push_back(EnemyBezier(pos,controlVec2,duration,border)); // borderは定数
    }

    void update(int deltatime) {
        for (size_t i = 0; i < list.size(); ) {
            if (!list[i].update(deltatime)) {
                list[i] = std::move(list.back());
                list.pop_back();
            } else ++i;
        }
    }

    void draw(const Renderer* renderer) const {
        for (const auto& bullet: list) {
            bullet.draw(renderer);
        }
    }
};