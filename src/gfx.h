#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include "def.h"
#include "SDL_def.h"

struct SpriteInfo {
    SpriteInfo(): tex(nullptr), w(0), h(0) {}
    SpriteInfo(SDL_Texture* tex, int width, int height):
      tex(tex), w(width), h(height) {}
    SDL_Texture* tex;
    int w;
    int h;
};


class Renderer {
	void* native;
    SpriteInfo spriteTable[static_cast<size_t>(SPR::count)];

public:
	Renderer(void* sdlRenderer);
    ~Renderer();

    Vec2 getSpriteSize(SPR spriteID);
	void drawSprite(SPR spriteID, const Vec2& pos) const;
	void drawSprite(SPR spriteID, const Vec2F& pos) const;
};
