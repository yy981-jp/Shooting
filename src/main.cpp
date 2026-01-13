#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <yy981/time.h>

#include "Game.h"


Game* game;

namespace param {
    constexpr int fpsDelayMS = 1000 / 60; // 60fps
}

void eventloop() {
	SDL_Event event;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
            }
            sleepc(tu::l, param::fpsDelayMS);
        }
	}
}


int main() {
	if (SDL_Init(SDL_INIT_VIDEO)) throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    game = new Game(800,680);

    eventloop();

    delete game;
    SDL_Quit();
}