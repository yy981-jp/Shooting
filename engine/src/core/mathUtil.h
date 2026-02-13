#pragma once
#include "vec2.h"

inline float dot(const vec2f& a, const vec2f& b) {
    return a.x * b.x + a.y * b.y;
}
