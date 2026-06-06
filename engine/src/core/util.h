#pragma once
#include "def.h"

inline bool isOffScreen(vec2f pos, vec2f spriteHalf) {
    return 
        pos.y-spriteHalf.y >= SCREEN.y || pos.y+spriteHalf.y <= -SCREEN.y ||
        pos.x-spriteHalf.x >= SCREEN.x || pos.x+spriteHalf.x <= -SCREEN.x;
}

inline void applyDamage(uint16_t& hp, uint16_t damage) {
	if (hp <= damage) hp = 0;
        else hp -= damage;
}
