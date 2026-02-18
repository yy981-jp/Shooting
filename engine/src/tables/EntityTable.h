#pragma once
#include <array>
#include <string_view>
#include <cstddef>

enum class EntityType : size_t {
#define X(name) name,
#include "../../../assets/entityTable.def"
#undef X
    Count
};

constexpr std::array<std::string_view,
    static_cast<size_t>(EntityType::Count)> entityNames = {
#define X(name) #name,
#include "../../../assets/entityTable.def"
#undef X
};
