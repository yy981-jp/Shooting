#pragma once
#include "def.h"


enum class SHTKeyCode: uint8_t {
    up      = 1 << 0,
    down    = 1 << 1,
    left    = 1 << 2,
    right   = 1 << 3,
    z       = 1 << 4,
    x       = 1 << 5,
    shift   = 1 << 6
};

constexpr bool has(uint8_t keys, SHTKeyCode k) {
    return keys & static_cast<uint8_t>(k);
}
