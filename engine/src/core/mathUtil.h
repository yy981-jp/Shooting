#pragma once
#include "vec2.h"

#include <numbers>

constexpr float dot(const vec2f& a, const vec2f& b) {
    return a.x * b.x + a.y * b.y;
}

constexpr float deg2rad(float deg) {
    return deg * (std::numbers::pi / 180.0f);
}

constexpr float rad2deg(float rad) {
    return rad * (180.0f / std::numbers::pi);
}
