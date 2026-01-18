#include <string>
#include <stdexcept>

#include "time.h"
#include "../graphics/gfx.h"
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

    struct KeyStat {
        bool
            up      = false,
            down    = false,
            left    = false,
            right   = false,
            z       = false,
            shift   = false;
    } keyStat;

    Renderer* renderer;
    Player* player;
    PlayerBullet_Manager playerBullet_Manager;
    ElapsedTime elapsedTime;

    void loadEntityTable();

public:
    Game(int windowWidth, int windowHeight, int FPSDelayMS);
    void update();
    void draw() const;
    void onKeyDown(const SDL_KeyboardEvent& e);
    void onKeyUP(const SDL_KeyboardEvent& e);
};
