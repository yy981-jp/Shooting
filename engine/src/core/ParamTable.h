#pragma once
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "def.h"
#include "util.h"
#include "rapidString.h"


class ParamTable {
public:
    rj::Document json;

    ParamTable() { // 各種tableを初期化
        json = readJson(Assets+"eventTable.json");
        // BezierCurve
        auto& bc = json["BezierCurves"];
        for (const auto& [key,values]: bc.GetObject()) {
            std::vector<Vec2F> curve;
            for (auto itr = json.Begin(); itr != json.End(); itr+=2) {
                Vec2F pos;
                pos.x = itr->GetInt();
                pos.y = (itr+1)->GetInt();
                curve.emplace_back(pos);
            }
            bezierCurve.table[key.GetString()] = curve;
        }
    }

    // ベジェ曲線の制御点
    struct BezierCurve {
        std::span<const Vec2F> get(std::string_view name) const {
            auto it = table.find(name);
            if (it == table.end()) throw std::runtime_error("BezierCurve not found");
            return it->second;
        }
        std::unordered_map<std::string, std::vector<Vec2F>, StrHash, StrEq> table;
    } bezierCurve;



};
