#include "cache.h"

#include <cmath>
#include <numbers>


CacheSV::CacheSV() {
    for (int i = 0; i < 3600; ++i) {
        double rad = i * std::numbers::pi / 180.0;
        move[i][0] = std::cos(rad);
        move[i][1] = -std::sin(rad);
    }
}

vec2f CacheSV::getDir(int rotate) const {
    return {move[rotate][0],move[rotate][1]};
}

float CacheSV::getSin(float rad) const {
    int angle = (rad * 180.0 * 10) / std::numbers::pi;
    return -move[angle][1];
}

float CacheSV::getCos(float rad) const {
    int angle = (rad * 180.0 * 10) / std::numbers::pi;
    return move[angle][0];
}


CacheSV cachesv;
