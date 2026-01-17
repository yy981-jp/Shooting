#include "player.h"

void Player::update(int dx, int dy, bool slow) {
    auto& v = moveTable[dy + 1][dx + 1];
    pos.x += v[0] * speed * (slow ? 0.5f : 1.0f);
    pos.y += v[1] * speed * (slow ? 0.5f : 1.0f);
}

void Player::draw(const Renderer* renderer) const {
    renderer->drawSprite(SPR::Player, pos);
}