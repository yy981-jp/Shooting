#pragma once
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "def.h"
#include "util.h"


class ParamTable {
    struct StrHash {
        using is_transparent = void;
        
        size_t operator()(std::string_view s) const noexcept {
            return std::hash<std::string_view>{}(s);
        }
    };
    
    struct StrEq {
        using is_transparent = void;
        
        bool operator()(std::string_view a, std::string_view b) const noexcept {
            return a == b;
        }
    };
    
public:
    ParamTable() { // 各種tableを初期化
        auto j = readJson(Assets+"eventTable.json");
        // BezierCurve
        auto& bc = j["BezierCurves"];
        for (const auto& [key,values]: bc.GetObject()) {
            std::vector<Vec2F> curve;
            for (auto itr = j.Begin(); itr != j.End(); itr+=2) {
                Vec2F pos;
                pos.x = itr->GetInt();
                pos.y = (itr+1)->GetInt();
                curve.emplace_back(pos);
            }
            bezierCurve.table[key.GetString()] = curve;
        }
    }

    struct BezierCurve {
        std::vector<Vec2F> get(std::string_view name) {
            // キャッシュ命中
            auto it = cache.find(name);
            if (it != cache.end())
                return it->second;

            // 元テーブルから取得
            auto src = table.find(std::string(name));
            if (src == table.end())
                throw std::runtime_error("Unknown entity: " + std::string(name));

            std::vector<Vec2F> id = src->second;
            cache.emplace(src->first, id);
            return id;
        }
        std::unordered_map<std::string, std::vector<Vec2F>, StrHash, StrEq> table;
    private:
        std::unordered_map<std::string_view, std::vector<Vec2F>, StrHash, StrEq> cache;
    } bezierCurve;



};
