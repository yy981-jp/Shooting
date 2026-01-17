#pragma once

#include <cmath>

#include "def.h"
#include "gfx.h"

class Player {
    int speed;
    float moveTable[3][3][2]; // y, x, [cos,sin]
    Vec2F pos;
public:
    Player(int speed): speed(speed), pos(400, 300) {
        for (int y = -1; y <= 1; ++y) {
            for (int x = -1; x <= 1; ++x) {
                float& cx = moveTable[y + 1][x + 1][0];
                float& cy = moveTable[y + 1][x + 1][1];

                if (x == 0 && y == 0) {
                    cx = cy = 0.0f;
                    continue;
                }

                float len = std::sqrt(float(x*x + y*y));
                cx = x / len;
                cy = y / len;
            }
        }
    }

    void update(int dx, int dy, bool slow);
    void draw(const Renderer* renderer) const;
};