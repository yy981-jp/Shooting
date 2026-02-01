#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <yy981/time.h>

#include "core/Game.h"
#include "core/time.h"
#include "VM/VM.h"
#include "enemy/enemyBezier.h"

Game* game;

void eventloop() {
	SDL_Event event;
	bool quit = false;
	FpsCounter fpsc;
	float displayFps = 0;
	while (!quit) {
		fpsc.update();
		game->update(displayFps);
		game->draw();
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: quit = true; break;
				case SDL_KEYDOWN: game->onKeyDown(event.key); break;
				case SDL_KEYUP: game->onKeyUP(event.key); break;
            }
        }
		displayFps = fpsc.getFps();

		// minimal delay to avoid 100% CPU usage; timing is driven by deltaTime in Game
		SDL_Delay(1);
	}
}


int main() {
	CheckTime ct;
	if (SDL_Init(SDL_INIT_VIDEO)) throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
	if (!(IMG_Init(IMG_INIT_PNG)&IMG_INIT_PNG)) throw std::runtime_error(std::string("SDL_IMG_Init failed: ") + SDL_GetError());
    game = new Game(800,680);

    eventloop();

    delete game;
    SDL_Quit();
	IMG_Quit();
}
