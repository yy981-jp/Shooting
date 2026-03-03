#pragma once

#include "exec.h"

class PlayScene;

namespace commandExec {

struct playScene : public base {
    using base::operator();
    PlayScene& scene;

    void operator()(const cmd::enemyBezier& c) const;
    void operator()(const cmd::simpleBullet& c) const;
    void operator()(const cmd::playerBullet& c) const;
};

}
