#pragma once

#include <string>
#include <stdexcept>

#include "time.h"
#include "collider.h"
#include "../gcms/gcms.h"
#include "../graphics/gfx.h"
#include "../audio/sfx.h"
#include "../scenes/scene.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>


class Game {
    SDL_Window* window;
    SDL_Renderer* rendererNative;
    SDL_Texture* texture;
    
    bool running = true;
    float displayFps = 0;

    Renderer* renderer;
    SFXManager* sfxMgr;
    ElapsedTime elapsedTime;
    FpsCounter fpsc;
    GCMS gcm;

    uint8_t keyStat; // SHTKeyCode
    SceneContext ctx;
    IScene* currentScene = nullptr;

    void update();
    void draw() const;
    void onKeyDown(const SDL_KeyboardEvent& e);
    void onKeyUP(const SDL_KeyboardEvent& e);

public:

    Game(const int windowWidth, const int windowHeight);
    ~Game();
    void tick();
    bool shouldQuit() {return !running;}

    // helper to avoid exposing sfxMgr to external code
    void playSfx(SFXID id) { ctx.sfx->play(id); }
};
