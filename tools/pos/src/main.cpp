#define SDL_MAIN_HANDLED

#include <iostream>
#include <stdexcept>

#include <SDL.h>
#include <SDL_image.h>

struct vec2s { int x, y; };

void onMouseButton(const SDL_MouseButtonEvent& e, vec2s windowHalf) {
    std::cout << "[" << e.x-windowHalf.x << "," << e.y-windowHalf.y << "],\n";
}

int main(int argc, char* argv[]) {
    vec2s windowSize;
    if (argc != 3)
         windowSize = {800,800};
    else windowSize = {std::stoi(argv[1]), std::stoi(argv[2])};

    vec2s windowHalf = { windowSize.x/2, windowSize.y/2};

    SDL_Window* window = SDL_CreateWindow(
        "SHT_dev-pos",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowSize.x,
        windowSize.y,
        SDL_WINDOW_SHOWN
    );

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: return 0;
    			case SDL_MOUSEBUTTONDOWN:
    				onMouseButton(event.button,windowHalf);
				break;
            }
        }
    }
}
