#pragma once
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include "rapidString.h"


class EntityTable {
public:
    uint16_t get(std::string_view name) const {
        auto it = table.find(name);
        if (it == table.end()) throw std::runtime_error("EntityTable: not found");
        return it->second;
    }

    std::unordered_map<std::string, uint16_t, StrHash, StrEq> table;
};
