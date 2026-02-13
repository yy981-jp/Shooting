#pragma once

#include "../core/vec2.h"


class Cache { 
    float move[3600][2];
public:
    vec2f getDir(int rotate) const;
    Cache();
};
