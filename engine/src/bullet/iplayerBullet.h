#pragma once

struct IPlayerBullet {
    IPlayerBullet(uint16_t attackPower): attackPower(attackPower) {}
    uint16_t attackPower = 0;
};
