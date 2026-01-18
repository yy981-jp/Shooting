#include "gfx.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdexcept>


SpriteInfo loadSprite(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* s = IMG_Load(path.c_str());
    if (!s) {
        throw std::runtime_error(std::string("IMG_Load error: ") + IMG_GetError());
    }

    int width  = s->w;
    int height = s->h;

    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    if (!t) {
        throw std::runtime_error(std::string("SDL_CreateTextureFromSurface") + IMG_GetError());
    }
    SDL_FreeSurface(s);

    return SpriteInfo{t, width, height};
}


/*------------------------------**
**          Renderer            **
**------------------------------*/
    Renderer::Renderer(void* sdlRenderer): native(sdlRenderer), spriteTable(entityTable.size()) {
        auto* renderer = static_cast<SDL_Renderer*>(native);

        for (const auto& [entities,id]: entityTable) {
            spriteTable[id] = loadSprite(Assets + "image/" + entities + ".png", renderer);
        }
    }

    Renderer::~Renderer() {}

    Vec2 Renderer::getSpriteSize(entityID spriteID) {
        SpriteInfo sprite = spriteTable[static_cast<size_t>(spriteID)];
        Vec2 vec;
        vec.x = sprite.w;
        vec.y = sprite.h;
        return vec;
    }

    void Renderer::drawSprite(entityID spriteID, const Vec2& pos) const {
        auto* renderer = static_cast<SDL_Renderer*>(native);
        if (!renderer) return;

        int width, height;
        SpriteInfo sprite = spriteTable[static_cast<size_t>(spriteID)];

        SDL_Rect dst;
        dst.x = static_cast<int>(pos.x);
        dst.y = static_cast<int>(pos.y);
        dst.w = sprite.w;
        dst.h = sprite.h;

        SDL_RenderCopy(renderer, sprite.tex, nullptr, &dst);
    }

    void Renderer::drawSprite(entityID spriteID, const Vec2F& posF) const {
        Vec2 pos;
        pos.x = posF.x;
        pos.y = posF.y;
        drawSprite(spriteID,pos);
    }
