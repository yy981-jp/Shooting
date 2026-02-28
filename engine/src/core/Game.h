#pragma once

#include <string>
#include <stdexcept>

#include "time.h"
#include "collider.h"
#include "commands.h"
#include "../graphics/gfx.h"
#include "../audio/sfx.h"
#include "../VM/VM.h"
#include "../player/player.h"
#include "../bullet/playerBullet.h"
#include "../bullet/simpleBullet.h"
#include "../enemy/enemyBezier.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>


class Game {
    SDL_Window* window;
    SDL_Renderer* rendererNative;
    SDL_Texture* texture;
    
    const std::string stgdatpath = Assets + "main.stg.dat";
    bool running = true;
    float displayFps = 0;

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
    SFXManager* sfxMgr;
    ElapsedTime elapsedTime;
    FpsCounter fpsc;
    GCMS gcm;

    Player* player;

    void update();
    void draw() const;
    void onKeyDown(const SDL_KeyboardEvent& e);
    void onKeyUP(const SDL_KeyboardEvent& e);
    void commandExec();

public:
    PlayerBullet_Manager* playerBullet_Manager;
    EnemyBezier_Manager* enemyBezier_Manager;
    SimpleBullet_Manager* simpleBullet_Manager;

    Game(const int windowWidth, const int windowHeight);
    ~Game();
    void tick();
    bool shouldQuit() {return !running;}

    // helper to avoid exposing sfxMgr to external code
    void playSfx(SFXID id) { sfxMgr->play(id); }
};
