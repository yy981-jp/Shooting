#pragma once
#include <array>
#include <string_view>
#include <cstddef>

enum class SpriteID : size_t {
#define X(name) name,
#include "../../../assets/entityTable.def"
#undef X
    Count
};

constexpr std::array<std::string_view,
    static_cast<size_t>(SpriteID::Count)> entityNames = {
#define X(name) #name,
#include "../../../assets/entityTable.def"
#undef X
};
