#include <string>
#include <stdexcept>

#include "collider.h"
#include "../scene/stageScene.h"

#include <SDL.h>
#include <SDL_image.h>


class Game {
    SDL_Window* window;
    SDL_Renderer* rendererNative;
    SDL_Texture* texture;

    static constexpr int
        widthULB = 800,         // 画面横幅, ULB座標系(左上を原点とする)の横方向の最大値
        heightULB = 600,        // 画面縦幅, ULB座標系の縦方向の最大値
        width = widthULB/2,     // 論理座標系(中央を原点とする)の横方向の最大値
        height = heightULB/2;   // 論理座標系の縦方向の最大値
    
    bool running = true;
    float displayFps = 0;

    VM* vm;
    Renderer* renderer;

    StageScene stageScene;

    void update();
    void draw() const;
    void onKeyDown(const SDL_KeyboardEvent& e);
    void onKeyUP(const SDL_KeyboardEvent& e);

public:
    Game(const int windowWidth, const int windowHeight);
    ~Game();
    void tick();
    bool shouldQuit() {return !running;}
};
