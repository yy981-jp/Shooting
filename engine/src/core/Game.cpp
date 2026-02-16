#include "Game.h"
#include "fsutil.h"
#include "json.h"
#include "collider.h"
#include "commands.h"


vec2i makeDir(bool up, bool down, bool left, bool right) {
	return {
		(right ? 1 : 0) - (left ? 1 : 0),
		(down  ? 1 : 0) - (up   ? 1 : 0)
	};
}


    Game::Game(const int windowWidth, const int windowHeight) {
        // SDL init
        if (SDL_Init(SDL_INIT_VIDEO)) throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
        if (!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)) throw std::runtime_error(std::string("SDL_IMG_Init failed: ") + SDL_GetError());

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

        texture = SDL_CreateTexture(
            rendererNative,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            widthULB,
            heightULB
        );
        if (!texture) throw std::runtime_error(std::string("SDL_CreateTexture failed: ") + SDL_GetError());

        SDL_RenderSetLogicalSize(rendererNative, widthULB, heightULB);
        
        cache = new Cache;
        // entity
        renderer = new Renderer(rendererNative, width, height);
        player = new Player(renderer, 5.0f*60.0f);
        playerBullet_Manager = new PlayerBullet_Manager(renderer);
        enemyBezier_Manager = new EnemyBezier_Manager(renderer);
        simpleBullet_Manager = new SimpleBullet_Manager(renderer, *cache);

        // VM
        vm = new VM(stgdatpath);
    }

    Game::~Game() {
        SDL_Quit();
    	IMG_Quit();
    }

    void Game::commandExec(const GameCommand& c) {
        if (!c.enable) return; // 有効ではないコマンドは弾く
        std::visit(commandExec_core{*this}, c.c);
    }
    void Game::commandExec(const GameCommands& cs) {
        for (auto& c: cs) commandExec(c);
    }

    void Game::update() {
        if (!elapsedTime) elapsedTime.init();
        int deltatime = elapsedTime.get();
        // VM step
        if (vm->running) {
            auto vm_r = vm->step();
            switch (vm_r) {
                using enum VM::ReturnCode;
                case success: finished: break;
                case error: throw std::runtime_error("VMで何らかの異常が発生しました"); break;
                case finished: break; // 現状何もする必要はない
                case spawnRequest: commandExec(vm->gamecommand); break;
            }
        }

        // entity update
        vec2i d = makeDir(keyStat.up, keyStat.down, keyStat.left, keyStat.right);
        commandExec( player->update(deltatime, d.x, d.y, keyStat.shift, keyStat.z) );
        if (!player->isAllive()) running = false;
        playerBullet_Manager->update(deltatime);
        commandExec( enemyBezier_Manager->update(deltatime) );
        simpleBullet_Manager->update(deltatime);

        physWorld.step(); // 当たり判定
    }
    
    void Game::draw() const {
        renderer->drawSprite(entityTable.get("background"), vec2i(-width,-height));
        player->draw(renderer);
        playerBullet_Manager->draw(renderer);
        enemyBezier_Manager->draw(renderer);
        simpleBullet_Manager->draw(renderer);

        // DEBUG
        physWorld.draw(renderer);

        SDL_RenderPresent(rendererNative);
    }

    void Game::onKeyDown(const SDL_KeyboardEvent& e) {
        if (e.repeat) return;
        switch (e.keysym.sym) {
            case SDLK_UP: keyStat.up = true; break;
            case SDLK_DOWN: keyStat.down = true; break;
            case SDLK_LEFT: keyStat.left = true; break;
            case SDLK_RIGHT: keyStat.right = true; break;
            case SDLK_z: keyStat.z = true; break;
            case SDLK_LSHIFT: keyStat.shift = true; break;
        }
    }

    void Game::onKeyUP(const SDL_KeyboardEvent& e) {
        switch (e.keysym.sym) {
            case SDLK_UP: keyStat.up = false; break;
            case SDLK_DOWN: keyStat.down = false; break;
            case SDLK_LEFT: keyStat.left = false; break;
            case SDLK_RIGHT: keyStat.right = false; break;
            case SDLK_z: keyStat.z = false; break;
            case SDLK_LSHIFT: keyStat.shift = false; break;
        }
    }

    void Game::tick() {
        if (running) {
            fpsc.update();
            SDL_SetWindowTitle(window,(std::to_string(displayFps) + "fps").c_str());
            update();
            draw();
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: running = false; break;
                case SDL_KEYDOWN: onKeyDown(event.key); break;
                case SDL_KEYUP: onKeyUP(event.key); break;
            }
        }

        if (running) displayFps = fpsc.getFps();
    }

    