#include "cache.h"

#include <cmath>
#include <numbers>


    Cache::Cache() {
        for (int i = 0; i < 3599; ++i) {
            double rad = i * std::numbers::pi / 180.0;
            move[i][0] = std::cos(rad);
            move[i][1] = -std::sin(rad);
        }
    }

    vec2f Cache::getDir(int rotate) const {
        return {move[rotate][0],move[rotate][1]};
    }