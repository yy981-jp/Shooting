#define SDL_MAIN_HANDLED

#include "core/Game.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


Game* game = nullptr;

void main_loop() {
    if (game->shouldQuit()) exit(0);
    game->tick();
}


int main(int argc, char* argv[]) {
    bool fullScrernMode = false;
    if (argc >= 2 && std::string(argv[1]) == "f") fullScrernMode = true;
    game = new Game(800,800,fullScrernMode);

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (!game->shouldQuit()) {
        game->tick();
        SDL_Delay(1);
    }
    delete game;
#endif
}
