#pragma once

#include <cmath>

#include "../core/def.h"
#include "../graphics/gfx.h"


struct ShotRequest {
	bool shouldShoot = false;
	Vec2 spawnPos{};
};

class Player {
    int speed;
    float moveTable[3][3][2]; // y, x, [cos,sin]
    Vec2F pos;
    Vec2 border;
    Vec2 spriteSize;
    int shootInterval = 20; // ms
    int elapsedTime = 0;

public:
    Player(int speed, Vec2 border, Vec2 spriteSize);

    ShotRequest update(int deltaTime, int dx, int dy, bool slow, bool shot);
    void draw(const Renderer* renderer) const;
};
