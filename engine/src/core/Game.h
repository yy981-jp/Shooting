#include <string>
#include <stdexcept>

#include "time.h"
#include "../graphics/gfx.h"
#include "../VM/VM.h"
#include "../player/player.h"
#include "../bullet/playerBullet.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


class Game {
    SDL_Window* window;
    SDL_Renderer* rendererNative;
    SDL_Texture* texture;

    static constexpr int
        width = 800,
        height = 600;
    
        const std::string stgdatpath = Assets + "main.stg.dat";

    struct KeyStat {
        bool
            up      = false,
            down    = false,
            left    = false,
            right   = false,
            z       = false,
            shift   = false;
    } keyStat;

    VM* vm;
    Renderer* renderer;
    Player* player;
    PlayerBullet_Manager playerBullet_Manager;
    ElapsedTime elapsedTime;

public:
    Game(const int windowWidth, const int windowHeight);
    void update();
    void draw() const;
    void onKeyDown(const SDL_KeyboardEvent& e);
    void onKeyUP(const SDL_KeyboardEvent& e);
};
