#pragma once

#include "../core/def.h"
#include "../core/collider.h"
#include "../core/commands.h"
#include "../core/spawnManager.h"
#include "../graphics/gfx.h"


class Player: public ICollidable {
    float speed; // pixels per second
    float moveTable[3][3][2]; // y, x, [cos,sin]
    vec2f pos;
    vec2f spriteHalf;
    int remainingLives;
    spawnManager spm;
    ColliderHandle h;
    void onHit(const CollisionInfo& info) override;

public:
    Player(const Renderer* r, float speed, int remainingLives = 5);
    bool isAllive() { return !remainingLives; }

    void update(int deltaTime, GCMS& gcm, int dx, int dy, bool slow, bool shot);
    void draw(const Renderer* renderer) const;
};
