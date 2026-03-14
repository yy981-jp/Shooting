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
#include <SDL_ttf.h>


class Game {
    SDL_Window* window;
    SDL_Renderer* nativeRenderer;
    SDL_Texture* texture;
    
    bool running = true;
    float displayFps = 0;

    Renderer* renderer;
    SFXManager* sfxMgr;
    ElapsedTime elapsedTime;
    FpsCounter fpsc;

    uint8_t keyStat; // SHTKeyCode
    SceneContext ctx;
    std::unique_ptr<IScene> currentScene = nullptr;

    void update();
    void draw() const;
    void onKeyDown(const SDL_KeyboardEvent& e);
    void onKeyUP(const SDL_KeyboardEvent& e);

public:
    GCMS gcm;
    void setScene(SceneID id);
    
    Game(SceneID initScene, bool fullscreen);
    ~Game();
    void tick();
    bool shouldQuit() {return !running;}

    // helper to avoid exposing sfxMgr to external code
    void playSfx(SFXID id) { ctx.sfx->play(id); }
};
