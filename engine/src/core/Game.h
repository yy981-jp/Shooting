#include <string>
#include <stdexcept>

#include "time.h"
#include "collider.h"
#include "../graphics/gfx.h"
#include "../VM/VM.h"
#include "../player/player.h"
#include "../bullet/playerBullet.h"
#include "../bullet/simpleBullet.h"
#include "../enemy/enemyBezier.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


class Game {
    SDL_Window* window;
    SDL_Renderer* rendererNative;
    SDL_Texture* texture;

    static constexpr int
        widthULB = 800,         // 画面横幅, ULB座標系(左上を原点とする)の横方向の最大値
        heightULB = 600,        // 画面縦幅, ULB座標系の縦方向の最大値
        width = widthULB/2,     // 論理座標系(中央を原点とする)の横方向の最大値
        height = heightULB/2;   // 論理座標系の縦方向の最大値
    
    const std::string stgdatpath = Assets + "main.stg.dat";

    uint64_t tick = 0;

    struct commandExec_core {
        Game& game;

        void operator()(const cmd::enemyBezier& c) const { game.enemyBezier_Manager->generate(vec2f(c.x,c.y),c.pattern,c.duration); }
        void operator()(const cmd::simpleBullet& c) const { game.simpleBullet_Manager->generate(c.pos,c.rotate,c.speed); }
    };

    struct KeyStat {
        bool
            up      = false,
            down    = false,
            left    = false,
            right   = false,
            z       = false,
            shift   = false;
    } keyStat;

    VM* vm;
    Renderer* renderer;
    ElapsedTime elapsedTime;
    Cache* cache;

    Player* player;
    PlayerBullet_Manager* playerBullet_Manager;
    EnemyBezier_Manager* enemyBezier_Manager;
    SimpleBullet_Manager* simpleBullet_Manager;

    void update(float displayFps);
    void draw() const;
    void onKeyDown(const SDL_KeyboardEvent& e);
    void onKeyUP(const SDL_KeyboardEvent& e);
    void commandExec(const GameCommand& c);
    void commandExec(const std::vector<GameCommand>& c);

public:
    Game(const int windowWidth, const int windowHeight);
    ~Game();
    void exec();
};
