#pragma once

#include "../core/def.h"
#include "../core/collider.h"
#include "../gcms/gcms.h"
#include "../core/spawnManager.h"
#include "../graphics/gfx.h"


class Player: public ICollidable {
    float speed; // pixels per second
    float moveTable[3][3][2]; // y, x, [cos,sin]
    static constexpr vec2f initPos = {0,350};
    vec2f pos = initPos;
    vec2f spriteHalf;
    spawnManager spm;
    spawnManager invincible;
    mutable spawnManager animation;
    ColliderHandle h;
    void onHit(const CollisionInfo& info, GCMS& gcm) override;

    mutable int currentFrame = 0;
    int frameNum;

public:
    uint64_t score = 0;
    int remainingLives = 500; // DEBUG

    Player(const Renderer* r, float speed);
    bool isAllive() { return remainingLives; }

    void update(float deltatime, GCMS& gcm, int dx, int dy, bool slow, bool shot);
    void draw(const Renderer* renderer) const;
};
