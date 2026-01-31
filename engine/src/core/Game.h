#include <string>
#include <stdexcept>

#include "time.h"
#include "../graphics/gfx.h"
#include "../VM/VM.h"
#include "../player/player.h"
#include "../bullet/playerBullet.h"
#include "../enemy/enemyBezier.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


class Game {
    SDL_Window* window;
    SDL_Renderer* rendererNative;
    SDL_Texture* texture;

    static constexpr int
        width = 800,
        height = 600;
    
        const std::string stgdatpath = Assets + "main.stg.dat";

    struct commandExec {
        Game& game;

        void operator()(const command_enemyBezier& c) const {
            game.enemyBezier_Manager->generate(vec2i(c.x,c.y),c.pattern,c.duration);
        }
        // void operator()(const Spawn& s) const { /* ... */ }
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

    Player* player;
    PlayerBullet_Manager playerBullet_Manager;
    EnemyBezier_Manager* enemyBezier_Manager;

public:
    Game(const int windowWidth, const int windowHeight);
    void update();
    void draw() const;
    void onKeyDown(const SDL_KeyboardEvent& e);
    void onKeyUP(const SDL_KeyboardEvent& e);
};
