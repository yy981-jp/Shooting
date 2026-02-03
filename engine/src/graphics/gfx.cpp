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
    Renderer::Renderer(void* sdlRenderer, int halfWidth, int halfHeight)
      : halfWidth(halfWidth), halfHeight(halfHeight), native(sdlRenderer), spriteTable(entityTable.table.size()) {
        auto* renderer = static_cast<SDL_Renderer*>(native);

        for (const auto& [entities,id]: entityTable.table) {
            spriteTable[id] = loadSprite(Assets + "image/" + entities + ".png", renderer);
        }
    }

    Renderer::~Renderer() {}

    vec2i Renderer::getSpriteSize(EntityType spriteID) {
        SpriteInfo sprite = spriteTable[static_cast<size_t>(spriteID)];
        vec2i vec;
        vec.x = sprite.w;
        vec.y = sprite.h;
        return vec;
    }

    void Renderer::drawSprite(EntityType spriteID, const vec2i& pos) const {
        auto* renderer = static_cast<SDL_Renderer*>(native);
        if (!renderer) return;

        SpriteInfo sprite = spriteTable[static_cast<size_t>(spriteID)];

        SDL_Rect dst;
        dst.x = pos.x + halfWidth;
        dst.y = pos.y + halfHeight;
        dst.w = sprite.w;
        dst.h = sprite.h;

        SDL_RenderCopy(renderer, sprite.tex, nullptr, &dst);
    }

    void Renderer::drawSprite(EntityType spriteID, const vec2f& posF) const {
        drawSprite(spriteID,vec2i(posF));
    }
