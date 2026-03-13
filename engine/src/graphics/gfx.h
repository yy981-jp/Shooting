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

struct SDL_Vertex;
struct SDL_Color;

enum class AtlasID: uint8_t {
    null,
    sprite, font,
    Count
};

struct SpriteEntry {
	float u1, v1, u2, v2; // uv rect for GPU
	uint16_t hw, hh; // half width, half height
    AtlasID id;
};

struct Sprite {
    SpriteEntry* entries;
    float spf = std::numeric_limits<float>::quiet_NaN(); // second per frame
    uint16_t frameCount;
    inline SpriteEntry operator[](const size_t& index) const { return entries[index]; }
};

struct Color {
    uint8_t r, g, b, a;

    operator SDL_Color() const;
};

class Renderer {
	void* native;
    Sprite spriteTable[static_cast<size_t>(SpriteID::Count)];

	// ===== for batch buffer =====
	mutable std::vector<SDL_Vertex> vertexBuffer;
	mutable std::vector<int> indexBuffer;

    // atlas
	void* atlasTex[static_cast<size_t>(AtlasID::Count)];
    mutable AtlasID currentAtlas = AtlasID::null;

    // load helper
    void loadSpriteAtlas(const AtlasID id, const std::string atlasName);

public:
	Renderer(void* sdlRenderer, int halfWidth, int halfHeight);
    ~Renderer();

    vec2f getSpriteSize(SpriteID spriteID) const;
    vec2f getSpriteHalfSize(SpriteID spriteID) const;
    const Sprite& getSprite(SpriteID spriteid) const;

    void drawSprite(SpriteID spriteID, const vec2f& pos, float rad = 0, uint16_t frameIndex = 0) const; // write to buffer
    void flush() const;

    void* getNativePtr() { return native; }

    // for Debug
    void drawFilledCircle(const vec2f pos, float rad) const;
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const;
};
