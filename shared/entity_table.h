#pragma once

#include <cstdint>

// STG Spawn Entity Types (for VM)
// These IDs correspond to the entity definitions in entity_defs.xmac
enum class STGEntityID : uint16_t {
    #define ENTITY(name, fields) name,
    #include "entity_defs.xmac"
    Count
};

