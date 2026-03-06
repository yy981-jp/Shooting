#pragma once
#include "def.h"

#include <vector>


// 衝突層
enum class CollisionLayer: uint8_t {
    wall            = 1 << 0,
    player          = 1 << 1,
    playerBullet    = 1 << 2,
    enemy           = 1 << 3,
    enemyBullet     = 1 << 4
};

// CollisionSystemが衝突処理関数に情報を渡すのに使う構造体
// 衝突相手についての情報を持つ
struct CollisionInfo {
    CollisionLayer layer;
};

// 衝突に関する情報全部
struct HitEvent {
    EntityHandle a_handle, b_handle;
    CollisionInfo a_info, b_info;
};
using HitEvents = std::vector<HitEvent>;
