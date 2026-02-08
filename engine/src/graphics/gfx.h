#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "../tables/all.h"
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
    int halfWidth, halfHeight;

public:
	Renderer(void* sdlRenderer, int halfWidth, int halfHeight);
    ~Renderer();

    vec2i getSpriteSize(EntityType spriteID) const;
	void drawSprite(EntityType spriteID, const vec2i& pos) const;
	void drawSprite(EntityType spriteID, const vec2f& pos) const;

    void drawFilledCircle(const vec2f pos, float rad) const;
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const ;
};
