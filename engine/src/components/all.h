#pragma once
#include "../graphics/gfx.h"

struct Transform { vec2f pos, area; }; // pos = x,y | area = w,h
struct Velocity { float vx, vy; };
struct Sprite { EntityType spriteID; };
struct Bullet { float damage; };

class Player {
    
};
