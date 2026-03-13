#include "player.h"
#include "../core/entityManager.h"
#include "../tables/all.h"

#include <cmath>

/*--------------------------*/
/*          Player          */
/*--------------------------*/

void Player::update(float deltatime, GCMS& gcm, int dx, int dy, bool slow, bool shot) {
    auto& v = moveTable[dy + 1][dx + 1];
    float cx = v[0] * speed * (slow ? 0.5f : 1.0f) * deltatime;
    float cy = v[1] * speed * (slow ? 0.5f : 1.0f) * deltatime;
    pos.x += cx;
    pos.y += cy;

    // 範囲外であれば座標の変更を取り消し
    if (pos.y+spriteHalf.y >= SCREEN.y || pos.y-spriteHalf.y <= -SCREEN.y)
        pos.y -= cy;
    if (pos.x+spriteHalf.x >= SCREEN.x || pos.x-spriteHalf.x <= -SCREEN.x)
        pos.x -= cx;

    // まず経過時間を累積
    spm.update(deltatime);
    invincible.update(deltatime);
    animation.update(deltatime);

    if (shot)
        for (int i = 0; i < spm.get(); ++i) {
            cmd::playerBullet rec_c;
            rec_c.pos = pos;
            gcm(rec_c);
            gcm(cmd::sfx(SFXID::shot));
        }

    physWorld.setPos(h,pos);
}

void Player::draw(const Renderer* renderer) const {
    currentFrame += animation.get();
    currentFrame %= frameNum;
    renderer->drawSprite(SpriteID::player, pos, 0, currentFrame);
}

Player::Player(const Renderer* r, float speed):
    speed(speed), spm(50), invincible(500),
    animation(r->getSprite(SpriteID::player).spf*1000) {

    // prepare cache
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

    // get info
    spriteHalf = r->getSpriteHalfSize(SpriteID::player);
    const auto& info = r->getSprite(SpriteID::player);
    frameNum = info.frameCount;


    // entry handle
    EntityHandle e = entMgr.create();

    Collider col{};
    col.type = ColliderType::Circle;
    col.handle = e;
    col.layer = CollisionLayer::player;
    col.mask =
        static_cast<uint8_t>(CollisionLayer::enemy) |
        static_cast<uint8_t>(CollisionLayer::enemyBullet) |
        static_cast<uint8_t>(CollisionLayer::item);
    col.circle.center = pos;
    col.circle.r = 3.0f;

    h = physWorld.add(col);

    entMgr.setPtr(e,this);
}

void Player::onHit(const CollisionInfo& info, GCMS& gcm) {
    switch (info.layer) {
        using enum CollisionLayer;

        case item: {
            // 暫定
            score++;
        } break;

        case enemy: case enemyBullet: {
            if (!invincible.get()) break;
            --remainingLives;
            pos = initPos;
        } break;

        default: throw std::runtime_error("Player::onHit: switch");
    }
}
