#pragma once
#include <cstddef>

enum class ComponentIndex : size_t {
    #define X(name) name,
    #include "components.def"
    #undef X
    Count
};

template<typename T> struct ComponentID;

#define X(name) \
template<> struct ComponentID<name> { \
    static constexpr size_t value = static_cast<size_t>(ComponentIndex::name); \
};
#include "components.def"
#undef X
