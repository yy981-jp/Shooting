#pragma once

#include <cmath>
#include <numbers>


class Cache { 
public:
    float move[3600][2];
    
    Cache() {
        for (int i = 0; i < 3599; ++i) {
            double rad = i * std::numbers::pi / 180.0;
            move[i][0] = std::cos(rad);
            move[i][1] = -std::sin(rad);
        }
    }
};
