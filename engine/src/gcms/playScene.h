#pragma once

#include "exec.h"

class PlayScene;

namespace commandExec {

struct playScene : public base {
    using base::operator();
    PlayScene& scene;

    void operator()(const cmd::bezierEnemy& c) const;
    void operator()(const cmd::simpleBullet& c) const;
    void operator()(const cmd::pointBullet& c) const;
    void operator()(const cmd::playerBullet& c) const;
};

}
