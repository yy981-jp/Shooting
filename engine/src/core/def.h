#pragma once

#include <cstdint>
#include <string>

#include "vec2.h"

using EntityType = uint16_t;    // 種類
using EntityGen = uint16_t;    // 世代
using EntityID = uint32_t;      // 個体別
struct EntityHandle {
    EntityID id;
    EntityGen gen;
};
const std::string Assets = "../../assets/";
constexpr vec2f WINDOW{800,800};
constexpr vec2f SCREEN = WINDOW / 2;
