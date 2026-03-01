#include "Game.h"
#include "fsutil.h"
#include "json.h"
#include "collider.h"
#include "../gcms/exec.h"
#include "entityManager.h"
#include "../scenes/playScene.h"


Game::Game(const int windowWidth, const int windowHeight) {
    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    if (!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)) throw std::runtime_error(std::string("SDL_IMG_Init failed: ") + IMG_GetError());
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) throw std::runtime_error(std::string("SDL_IMG_Init failed: ") + Mix_GetError());
    Mix_AllocateChannels(64);

    window = SDL_CreateWindow(
        "Shooting-SDL2",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );
    if (!window) throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());

    rendererNative = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!rendererNative) throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());

    SDL_RenderSetLogicalSize(rendererNative, WINDOW.x, WINDOW.y);
    
    // entity
    renderer = new Renderer(rendererNative, SCREEN.x, SCREEN.y);
    sfxMgr = new SFXManager;

    ctx = SceneContext{ &gcm, &keyStat, renderer, sfxMgr };

    currentScene = new PlayScene(ctx);
}

Game::~Game() {
    SDL_Quit();
    IMG_Quit();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(rendererNative);
}
// #include <iostream>
// #include <format>
void Game::update() {
    if (!elapsedTime) elapsedTime.init();
    float deltatime = elapsedTime.get();

    currentScene->update(ctx,deltatime);

    // std::cout << std::format("{:0>60b}", *ctx.input) << std::endl;

    physWorld.step(); // 当たり判定

    for (const auto& c: ctx.gcms->get()) currentScene->handleCommand(c,*this);
    ctx.gcms->clear();
}

void Game::draw() const {
    ctx.gfx->drawSprite(EntityType::background, SCREEN * -1);

    currentScene->draw(ctx);

    // DEBUG
    // physWorld.draw(renderer);

    SDL_RenderPresent(rendererNative);
}

void Game::onKeyDown(const SDL_KeyboardEvent& e) {
    if (e.repeat) return;
    switch (e.keysym.scancode) {
        case SDL_SCANCODE_I:
        case SDL_SCANCODE_UP:       keyStat |= static_cast<uint8_t>(SHTKeyCode::up);    break;
        case SDL_SCANCODE_K:
        case SDL_SCANCODE_DOWN:     keyStat |= static_cast<uint8_t>(SHTKeyCode::down);  break;
        case SDL_SCANCODE_J:
        case SDL_SCANCODE_LEFT:     keyStat |= static_cast<uint8_t>(SHTKeyCode::left);  break;
        case SDL_SCANCODE_L:
        case SDL_SCANCODE_RIGHT:    keyStat |= static_cast<uint8_t>(SHTKeyCode::right); break;
        case SDL_SCANCODE_Z:        keyStat |= static_cast<uint8_t>(SHTKeyCode::z);     break;
        case SDL_SCANCODE_X:        keyStat |= static_cast<uint8_t>(SHTKeyCode::x);     break;
        case SDL_SCANCODE_LSHIFT:   keyStat |= static_cast<uint8_t>(SHTKeyCode::shift); break;
    }
    if (e.keysym.sym == SDLK_ESCAPE) exit(111);
}

void Game::onKeyUP(const SDL_KeyboardEvent& e) {
    switch (e.keysym.scancode) {
        case SDL_SCANCODE_I:
        case SDL_SCANCODE_UP:       keyStat &= ~static_cast<uint8_t>(SHTKeyCode::up);    break;
        case SDL_SCANCODE_K:
        case SDL_SCANCODE_DOWN:     keyStat &= ~static_cast<uint8_t>(SHTKeyCode::down);  break;
        case SDL_SCANCODE_J:
        case SDL_SCANCODE_LEFT:     keyStat &= ~static_cast<uint8_t>(SHTKeyCode::left);  break;
        case SDL_SCANCODE_L:
        case SDL_SCANCODE_RIGHT:    keyStat &= ~static_cast<uint8_t>(SHTKeyCode::right); break;
        case SDL_SCANCODE_Z:        keyStat &= ~static_cast<uint8_t>(SHTKeyCode::z);     break;
        case SDL_SCANCODE_X:        keyStat &= ~static_cast<uint8_t>(SHTKeyCode::x);     break;
        case SDL_SCANCODE_LSHIFT:   keyStat &= ~static_cast<uint8_t>(SHTKeyCode::shift); break;
    }
}

void Game::tick() {
    fpsc.update();
    SDL_SetWindowTitle(window,(std::to_string(displayFps) + "fps   " + std::to_string(entMgr.size()) + "ents").c_str());
    update();
    draw();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: running = false; break;
            case SDL_KEYDOWN: onKeyDown(event.key); break;
            case SDL_KEYUP: onKeyUP(event.key); break;
        }
    }

    displayFps = fpsc.getFps();

    // DEBUG
    // gcm(cmd::sfx(SFXID::shot));
}
