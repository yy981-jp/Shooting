#pragma once

#include <cstdint>
#include <string>

#include "vec2.h"

using EntityType = uint16_t;    // 種別
using EntityID = uint32_t;      // 個体別 (異種別との競合の可能性あり)
struct EntityHandle {
    EntityType type;
    EntityID id;
    uint64_t getHandle() {
        return (uint64_t(type) << 32) | uint64_t(id);
    }
};
inline constexpr std::string Assets = "../../assets/";
