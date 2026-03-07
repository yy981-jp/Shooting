#pragma once

#include <cstdint>
#include <vector>

#include "../core/def.h"
#include "../tables/all.h"


enum class SpriteID : size_t {
#define X(name) name,
#include "../../../assets/sprite.def"
#undef X
    Count
};

struct SpriteInfo {
    SpriteInfo(): tex(nullptr), hw(0), hh(0) {}
    SpriteInfo(void* tex, int width, int height):
      tex(tex), hw(width), hh(height) {}
    void* tex;
    float hw;
    float hh;
};

struct SDL_Vertex;

class Renderer {
	void* native;
    SpriteInfo spriteTable[static_cast<size_t>(SpriteID::Count)];

	// ===== for batch buffer =====
	mutable std::vector<SDL_Vertex> vertexBuffer;
	mutable std::vector<int> indexBuffer;
	mutable void* currentTexture = nullptr;

public:
	Renderer(void* sdlRenderer, int halfWidth, int halfHeight);
    ~Renderer();

    vec2i getSpriteSize(SpriteID spriteID) const;
    vec2f getSpriteHalfSize(SpriteID spriteID) const;
	void drawSprite(SpriteID spriteID, const vec2f& pos, float rad = 0) const; // write to buffer
    void flush() const;
	void drawSpriteNow(SpriteID spriteID, const vec2f& pos, float rad = 0, float scale = 1) const;

    // for Debug
    void drawFilledCircle(const vec2f pos, float rad) const;
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const;
};
