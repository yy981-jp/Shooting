#pragma once
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <span>

#include "../core/def.h"
#include "../core/json.h"
#include "../core/rapidString.h"


class ParamTable {
public:
    rj::Document json;

    ParamTable();

    // ベジェ曲線の制御点
    struct BezierCurve {
        std::span<const vec2f> get(std::string_view name) const;
        std::unordered_map<std::string, std::vector<vec2f>, StrHash, StrEq> table;
    } bezierCurve;



};
