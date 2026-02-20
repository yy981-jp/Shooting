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

    void Game::commandExec() {
        for (auto& c: gcm.get()) std::visit(commandExec_core{*this}, c);
        gcm.clear();
    }

    void Game::update() {
        if (!elapsedTime) elapsedTime.init();
        float deltatime = elapsedTime.get();

        // VM step
        if (vm->running) {
            switch (vm->step(gcm)) {
                using enum VM::ReturnCode;
                case success: break;
                case error: throw std::runtime_error("VMで何らかの異常が発生しました"); break;
            }
        }

        // entity update
        vec2i d = makeDir(keyStat.up, keyStat.down, keyStat.left, keyStat.right);
        player->update(deltatime, gcm, d.x, d.y, keyStat.shift, keyStat.z);
        // if (!player->isAllive()) running = false;
        playerBullet_Manager->update(deltatime);
        enemyBezier_Manager->update(deltatime,gcm);
        simpleBullet_Manager->update(deltatime);

        physWorld.step(); // 当たり判定

        commandExec();
    }
    
    void Game::draw() const {
        renderer->drawSprite(EntityType::background, vec2i(-width,-height));
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

            case SDLK_ESCAPE: exit(111);
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
    }

    