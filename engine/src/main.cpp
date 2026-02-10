#define SDL_MAIN_HANDLED

#include "core/Game.h"

int main() {
    Game game(800,600);
    game.exec();
}
