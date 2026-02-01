#pragma once

#include <cmath>

#include "../core/def.h"
#include "../graphics/gfx.h"


struct ShotRequest {
	bool shouldShoot = false;
	vec2i spawnPos{};
};

class Player {
    float speed; // pixels per second
    float moveTable[3][3][2]; // y, x, [cos,sin]
    vec2f pos;
    vec2i border, u_border;
    vec2i spriteSize;
    int shootInterval = 50; // ms
    int elapsedTime = 0;

public:
    Player(float speed, vec2i border, vec2i spriteSize);

    ShotRequest update(int deltaTime, int dx, int dy, bool slow, bool shot);
    void draw(const Renderer* renderer) const;
};
