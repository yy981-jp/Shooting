#pragma once

#include "../core/vec2.h"


class CacheSV { // CacheServer 
    float move[3800][2];
public:
    vec2f getDir(int rotate) const;
    float getSin(float rad) const;
    float getCos(float rad) const;
    CacheSV();
};

extern CacheSV cachesv;
