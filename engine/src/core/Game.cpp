#include "Game.h"
#include "fsutil.h"
#include "json.h"
#include "collider.h"
#include "../gcms/exec.h"
#include "entityManager.h"
#include "../scenes/playScene.h"
#include "../scenes/titleScene.h"


std::unique_ptr<IScene> createScene(SceneID id, SceneContext& ctx) {
	switch (id) {
		case SceneID::title:    return std::make_unique<TitleScene>(ctx);
		case SceneID::play:     return std::make_unique<PlayScene>(ctx);
	}
	return nullptr;
}


Game::Game(SceneID initScene, bool fullscreen) {
    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    if (!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)) throw std::runtime_error(std::string("SDL_IMG_Init failed: ") + IMG_GetError());
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) < 0) throw std::runtime_error(std::string("SDL_MIX_Init failed: ") + Mix_GetError());
    if (TTF_Init() == -1) throw std::runtime_error(std::string("SDL_TTF_Init failed: ") + TTF_GetError());

    Mix_AllocateChannels(64);

    window = SDL_CreateWindow(
        "SHT",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW.x,
        WINDOW.y,
        (fullscreen? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_SHOWN)
    );
    if (!window) throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    // int w_x, w_y;
    // SDL_GetWindowPosition(window, &w_x, &w_y);
    // SDL_SetWindowPosition(window, w_x + UI.x/2, w_y);

    nativeRenderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (!nativeRenderer) throw std::runtime_error(std::string("SDL_CreateRenderer failed: ") + SDL_GetError());

    SDL_RenderSetLogicalSize(nativeRenderer, WINDOW.x, WINDOW.y);
    SDL_SetRenderDrawBlendMode(nativeRenderer, SDL_BLENDMODE_BLEND);
    // SDL_Rect rect{0,0, static_cast<int>(WINDOW.x), static_cast<int>(WINDOW.y)};
    // SDL_RenderSetViewport(nativeRenderer, &rect);

    renderer = new Renderer(nativeRenderer, SCREEN.x, SCREEN.y);
    text = new Text(renderer);
    sfxMgr = new SFXManager;

    ctx = SceneContext{ &gcm, &keyStat, renderer, text, sfxMgr };

    setScene(initScene);
}

Game::~Game() {
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(nativeRenderer);
}

void Game::update() {
    if (!elapsedTime) elapsedTime.init();
    float deltatime = elapsedTime.get();

    currentScene->update(ctx,deltatime);

    // ハンドラー内で発行されたコマンドも処理するため、ループを繰り返す
    while (!ctx.gcms->get().empty()) {
        auto cmds = ctx.gcms->get();
        ctx.gcms->clear();
        for (const auto& c: cmds) currentScene->handleCommand(c,*this);
    }
}

void Game::draw() const {
    currentScene->draw(ctx);

    SDL_RenderPresent(nativeRenderer);
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
    gcm(cmd::notiFps(displayFps));
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

void Game::setScene(SceneID id) {
    if (!(currentScene = createScene(id,ctx)))
        throw std::runtime_error("createScene: nullptr");
}
