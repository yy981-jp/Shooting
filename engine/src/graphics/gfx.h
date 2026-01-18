#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "../core/def.h"
#include "../core/SDL_def.h"

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
    std::vector<SpriteInfo> spriteTable;

public:
	Renderer(void* sdlRenderer);
    ~Renderer();

    Vec2 getSpriteSize(entityID spriteID);
	void drawSprite(entityID spriteID, const Vec2& pos) const;
	void drawSprite(entityID spriteID, const Vec2F& pos) const;
};
