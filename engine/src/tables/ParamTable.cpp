#include "ParamTable.h"

#include "../core/fsutil.h"

    ParamTable::ParamTable() {
        // 各種tableを初期化
        json = readJson(Assets+"eventTable.json");
        // BezierCurve
        auto& bc = json["BezierCurves"];
        for (const auto& [key,values]: bc.GetObject()) {
            std::vector<vec2f> curve;
            for (const auto& p: values.GetArray()) {
                vec2f pos;
                pos.x = p[0].GetFloat();
                pos.y = p[1].GetFloat();
                curve.emplace_back(pos);
            }
            bezierCurve.table[key.GetString()] = curve;
        }
    }

        std::span<const vec2f> ParamTable::BezierCurve::get(std::string_view name) const {
            auto it = table.find(name);
            if (it == table.end()) throw std::runtime_error("BezierCurve not found - id: " + std::string(name));
            return it->second;
        }
