#include "player.h"


/*--------------------------*/
/*          Player          */
/*--------------------------*/

    ShotRequest Player::update(int deltaTime, int dx, int dy, bool slow, bool shot) {
        auto& v = moveTable[dy + 1][dx + 1];
        float cx = v[0] * speed * (slow ? 0.5f : 1.0f);
        float cy = v[1] * speed * (slow ? 0.5f : 1.0f);
        pos.x += cx;
        pos.y += cy;
        if (pos.y>=border.y || pos.y<=0 || pos.x>=border.x || pos.x<=0) {
            pos.x -= cx;
            pos.y -= cy;
        }
        ShotRequest req;
        if (shot && (elapsedTime > shootInterval)) {
            elapsedTime -= shootInterval;
            req.shouldShoot = true;
            req.spawnPos = Vec2(
                pos.x + (spriteSize.x/2),
                pos.y
            );
        } else elapsedTime += deltaTime;
        return req;
    }

    void Player::draw(const Renderer* renderer) const {
        renderer->drawSprite(entityTable["player"], pos);
    }

    Player::Player(int speed, Vec2 i_border, Vec2 spriteSize):
      speed(speed), spriteSize(spriteSize), pos(400, 300) {
        border.x = i_border.x - spriteSize.x;
        border.y = i_border.y - spriteSize.y;

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
