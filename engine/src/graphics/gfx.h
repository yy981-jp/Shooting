#pragma once

#include <cstdint>
#include <vector>

#include "../core/def.h"
#include "../tables/all.h"


struct SpriteInfo;

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
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const;
};
