#include "player.h"
#include "../tables/all.h"


/*--------------------------*/
/*          Player          */
/*--------------------------*/

    ShotRequest Player::update(int deltaTime, int dx, int dy, bool slow, bool shot) {
        auto& v = moveTable[dy + 1][dx + 1];
        float dtSec = static_cast<float>(deltaTime) / 1000.0f;
        float cx = v[0] * speed * (slow ? 0.5f : 1.0f) * dtSec;
        float cy = v[1] * speed * (slow ? 0.5f : 1.0f) * dtSec;
        pos.x += cx;
        pos.y += cy;

        // 範囲外であれば座標の変更を取り消し
        if (pos.y>=border.y || pos.y<=u_border.y) {
            pos.y -= cy;
        }
        if (pos.x>=border.x || pos.x<=u_border.x) {
            pos.x -= cx;
        }

        ShotRequest req;
        // まず経過時間を累積
        elapsedTime += deltaTime;
        int fireCount = 0;
        if (shot) {
            while (elapsedTime >= shootInterval) {
                elapsedTime -= shootInterval;
                ++fireCount;
            }
        }

        if (fireCount > 0) {
            req.shouldShoot = true;
            req.spawnPos = vec2i(
                pos.x + (spriteSize.x/2),
                pos.y
            );
        }
        return req;
    }

    void Player::draw(const Renderer* renderer) const {
        renderer->drawSprite(entityTable.get("player"), pos);
    }

    Player::Player(float speed, vec2i i_border, vec2i spriteSize):
      speed(speed), spriteSize(spriteSize), pos(0,0) {
        border.x = i_border.x - spriteSize.x;
        border.y = i_border.y - spriteSize.y;
        u_border.x = -i_border.x;
        u_border.y = -i_border.y;

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
