#pragma once

#include <cmath>

#include "../core/def.h"
#include "../core/collider.h"
#include "../graphics/gfx.h"


struct ShotRequest {
	bool shouldShoot = false;
	vec2i spawnPos{};
};

class Player: public ICollidable {
    float speed; // pixels per second
    float moveTable[3][3][2]; // y, x, [cos,sin]
    vec2f pos;
    vec2f spriteHalf;
    int shootInterval = 50; // ms
    int elapsedTime = 0;
    ColliderHandle h;

    void onHit(const CollisionInfo& info) {
        exit(200); // 一旦プログラム終了
    }

public:
    Player(const Renderer* r, float speed);

    ShotRequest update(int deltaTime, int dx, int dy, bool slow, bool shot);
    void draw(const Renderer* renderer) const;
};
