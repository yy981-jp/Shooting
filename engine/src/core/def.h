#pragma once

#include <cstdint>
#include <string>

#include "vec2.h"


const std::string Assets = "../assets/";
constexpr vec2f WINDOW{800,800};
constexpr vec2f SCREEN = WINDOW / 2;

using EntityGen = uint16_t;    // 世代
using EntityID = uint32_t;      // 個体別
struct EntityHandle {
    EntityID id;
    EntityGen gen;
};

struct MotionState {
    vec2f pos = {0,0};
    float angle = 0; // rad
    float speed = 0;
};

enum class SceneID {
	null,
	play,
	title
};
