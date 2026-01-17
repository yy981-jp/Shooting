#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>

#include <yy981/time.h>

#include "Game.h"
#include "time.h"


Game* game;

namespace param {
    constexpr int fpsDelayMS = 1000 / 60; // 60fps
}

void eventloop() {
	SDL_Event event;
	bool quit = false;
	while (!quit) {
		game->update();
		game->draw();
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: quit = true; break;
				case SDL_KEYDOWN: game->onKeyDown(event.key); break;
				case SDL_KEYUP: game->onKeyUP(event.key); break;
            }
            sleepc(tu::l, param::fpsDelayMS);
        }
	}
}


int main() {
	checkTime ct;
	if (SDL_Init(SDL_INIT_VIDEO)) throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
	if (!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)) throw std::runtime_error(std::string("SDL_IMG_Init failed: ") + SDL_GetError());
    game = new Game(800,680,param::fpsDelayMS);

	std::cout << "load: " << ct.end() << "ms\n";

    eventloop();

    delete game;
    SDL_Quit();
	IMG_Quit();
}
