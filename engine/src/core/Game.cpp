#include "Game.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <fstream>

namespace rj = rapidjson;

Vec2 makeDir(bool up, bool down, bool left, bool right) {
	return {
		(right ? 1 : 0) - (left ? 1 : 0),
		(down  ? 1 : 0) - (up   ? 1 : 0)
	};
}

    void Game::loadEntityTable() {
        std::ifstream ifs(Assets + "entity.def.json");
        if (!ifs) throw std::runtime_error("Game::loadEntityTable(): ifs");
        rj::IStreamWrapper isw(ifs);
        rj::Document doc;
        doc.ParseStream(isw);
        const rj::Value& obj = doc;

        for (auto itr = obj.MemberBegin(); itr != obj.MemberEnd(); ++itr) {
            const char* key = itr->name.GetString();
            const rj::Value& value = itr->value;
            entityTable[key] = value["id"].GetInt();
        }
    }

    Game::Game(int windowWidth, int windowHeight, int FPSDelayMS) {
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
            width,
            height
        );
        if (!texture) throw std::runtime_error(std::string("SDL_CreateTexture failed: ") + SDL_GetError());

        loadEntityTable();

        renderer = new Renderer(rendererNative);
        player = new Player(5,Vec2(width,height),renderer->getSpriteSize(entityTable["player"]));
        
        SDL_RenderSetLogicalSize(rendererNative, width, height);
    }

    void Game::update() {
        if (!elapsedTime) elapsedTime.init();
        int deltaTime = elapsedTime.get();
        Vec2 d = makeDir(keyStat.up, keyStat.down, keyStat.left, keyStat.right);
        ShotRequest playerShotReq = player->update(deltaTime, d.x, d.y, keyStat.shift, keyStat.z);
        if (playerShotReq.shouldShoot) playerBullet_Manager.generate(playerShotReq.spawnPos);
        playerBullet_Manager.update();
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

    void Game::draw() const {
        // SDL_RenderClear(rendererNative); (これがあると黒帯領域が発生する なんでかって? 未来の自分調べといて)
        renderer->drawSprite(entityTable["background"], Vec2(0,0));
        player->draw(renderer);
        playerBullet_Manager.draw(renderer);
        SDL_RenderPresent(rendererNative);
    }
