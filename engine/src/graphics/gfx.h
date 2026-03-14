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

enum class FontSize {
    f16, f32, f64,
    Count
};

constexpr std::string fontSizeName[static_cast<int>(FontSize::Count)] = {
    "16", "32", "64"
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
    constexpr static int ASCII_RANGE = 95;

	void* native;
    Sprite spriteTable[static_cast<size_t>(SpriteID::Count)];
    SpriteEntry fontTable[ASCII_RANGE * static_cast<size_t>(FontSize::Count)];

	// ===== for batch buffer =====
	mutable std::vector<SDL_Vertex> vertexBuffer;
	mutable std::vector<int> indexBuffer;

    // atlas
	void* atlasTex[static_cast<size_t>(AtlasID::Count)];
    mutable AtlasID currentAtlas = AtlasID::null;

    // load helper
    void loadSpriteAtlas(const AtlasID id, const std::string atlasName);
    void loadFontAtlas(const AtlasID id, const std::string atlasName);

    void setAtlas(const AtlasID id, void* tex) { atlasTex[static_cast<int>(id)] = tex; }

    constexpr static size_t getFontTableIndex(FontSize size, char c) {
		return static_cast<int>(size) * ASCII_RANGE + c - ' ';
    }

    void draw(SpriteEntry entry, const vec2f& pos, float rad) const;

public:
    /// @brief 外部からの実行は任意 draw関数内部で自動実行
    void flush() const;
	Renderer(void* sdlRenderer, int halfWidth, int halfHeight);
    ~Renderer();

    vec2f getSpriteSize(SpriteID spriteID) const;
    vec2f getSpriteHalfSize(SpriteID spriteID) const;
    const Sprite& getSprite(SpriteID spriteid) const;
    const SpriteEntry& getFontEntry(FontSize size, char c) const;

    void drawSprite(SpriteID spriteID, const vec2f& pos, float rad = 0, uint16_t frameIndex = 0) const; // write to buffer
    const SpriteEntry& drawFont(FontSize size, char c, const vec2f& pos, float rad = 0) const; // write to buffer

    // for Debug
    void drawFilledCircle(const vec2f pos, float rad) const;
    void setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const;
};
