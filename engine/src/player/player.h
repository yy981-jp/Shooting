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
    int remainingLives = 5;
    uint64_t score = 0;
    spawnManager spm;
    spawnManager invincible;
    ColliderHandle h;
    void onHit(const CollisionInfo& info, GCMS& gcm) override;

public:
    Player(const vec2f& spriteHalf, float speed);
    bool isAllive() { return remainingLives; }

    void update(float deltatime, GCMS& gcm, int dx, int dy, bool slow, bool shot);
    void draw(const Renderer* renderer) const;
};
