#include "gfx.h"
#include "util.h"
#include "../core/mathUtil.h"
#include "../core/cache.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdexcept>
#include <cmath>


constexpr std::array<std::string_view,
    static_cast<size_t>(SpriteID::Count)> entityNames = {
#define X(name,type) #name,
#include "../../../assets/sprite.def"
#undef X
};

enum class SpriteFileType {
	null, png, gif
};

SpriteFileType sprFTList[] = {
#define X(name,type) SpriteFileType::type,
#include "../../../assets/sprite.def"
#undef X
	SpriteFileType::null
};



Color::operator SDL_Color() const {
    return SDL_Color{r,g,b,a};
}


SpriteEntry loadSprite(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* s = IMG_Load(path.c_str());
    if (!s) throw std::runtime_error(std::string("IMG_Load error: ") + IMG_GetError());
	
	return createEntry(s,renderer);
}
/*
std::vector<std::string> parseU8String(const std::string& str) {
	int char_size;
	std::vector<std::string> result;

	for (size_t pos = 0; pos < str.size(); pos += char_size) {
		unsigned char lead = str[pos];
			 if (lead < 0x80) char_size=1;
		else if (lead < 0xE0) char_size=2;
		else if (lead < 0xF0) char_size=3;
		else char_size=4;

		result.push_back(str.substr(pos,char_size));
	}
}
*/

/*------------------------------**
**          Renderer            **
**------------------------------*/
Renderer::Renderer(void* sdlRenderer, int halfWidth, int halfHeight): native(sdlRenderer) {
    auto* renderer = static_cast<SDL_Renderer*>(native);
	
	// sprite 読み込み
    for (size_t i = 0; i < entityNames.size(); ++i) {
        auto name = entityNames[i];

        spriteTable[i] = loadSprite(
            Assets + "image/" + std::string(name) + ".png",
            renderer
        );
    }
}

Renderer::~Renderer() {
    for (size_t i = 0; i < entityNames.size(); ++i)
        SDL_DestroyTexture(static_cast<SDL_Texture*>(spriteTable[i].tex));
}

vec2f Renderer::getSpriteHalfSize(SpriteID spriteID) const {
    const SpriteEntry& sprite = spriteTable[static_cast<size_t>(spriteID)];
    vec2f vec;
    vec.x = sprite.hw;
    vec.y = sprite.hh;
    return vec;
}

vec2f Renderer::getSpriteSize(SpriteID spriteID) const {
    return getSpriteHalfSize(spriteID) * 2;
}

void Renderer::drawSprite(SpriteID spriteID, const vec2f& pos, float rad) const {
	const auto& sp = spriteTable[static_cast<size_t>(spriteID)];

	SDL_Texture* tex;

	float hw = sp.hw;
	float hh = sp.hh;

	float u1 = sp.u1;
	float v1 = sp.v1;
	float u2 = sp.u2;
	float v2 = sp.v2;

	int baseIndex = vertexBuffer.size();

	if (rad == 0.0f) {
		// ---- 回転なしルート ----
		SDL_Vertex v[4];

		v[0].position = { SCREEN.x + pos.x - hw, SCREEN.y + pos.y - hh };
		v[1].position = { SCREEN.x + pos.x + hw, SCREEN.y + pos.y - hh };
		v[2].position = { SCREEN.x + pos.x + hw, SCREEN.y + pos.y + hh };
		v[3].position = { SCREEN.x + pos.x - hw, SCREEN.y + pos.y + hh };

		v[0].tex_coord = { u1, v1 };
		v[1].tex_coord = { u2, v1 };
		v[2].tex_coord = { u2, v2 };
		v[3].tex_coord = { u1, v2 };

		for (int i = 0; i < 4; ++i) {
			v[i].color = {255,255,255,255};
			vertexBuffer.push_back(v[i]);
		}
	}
	else {
		// ---- 回転ありルート ----
		float c = cachesv.getCos(rad);
		float s = -cachesv.getSin(rad);

		SDL_FPoint corners[4] = {
			{-hw, -hh},
			{ hw, -hh},
			{ hw,  hh},
			{-hw,  hh}
		};

		const float u[4] = {u1, u2, u2, u1};
		const float v[4] = {v1, v1, v2, v2};

		for (int i = 0; i < 4; ++i) {
			float rx = corners[i].x * c - corners[i].y * s;
			float ry = corners[i].x * s + corners[i].y * c;

			SDL_Vertex vert;

			vert.position = {
				SCREEN.x + pos.x + rx,
				SCREEN.y + pos.y + ry
			};

			vert.color = {255,255,255,255};

			vert.tex_coord = { u[i], v[i] };

			vertexBuffer.push_back(vert);
		}
	}

	indexBuffer.push_back(baseIndex + 0);
	indexBuffer.push_back(baseIndex + 1);
	indexBuffer.push_back(baseIndex + 2);
	indexBuffer.push_back(baseIndex + 2);
	indexBuffer.push_back(baseIndex + 3);
	indexBuffer.push_back(baseIndex + 0);
}

void Renderer::drawSpriteNow(SpriteID spriteID, const vec2f& pos, float rad, float scale) const {
    auto* renderer = static_cast<SDL_Renderer*>(native);
    if (!renderer) return;

    SpriteEntry sprite = spriteTable[static_cast<size_t>(spriteID)];

    SDL_FRect dst;
    dst.x = pos.x + SCREEN.x - sprite.hw * scale;
    dst.y = pos.y + SCREEN.y - sprite.hh * scale;

	dst.w = sprite.hw * 2 * scale;
    dst.h = sprite.hh * 2 * scale;

    SDL_RenderCopyExF(renderer, static_cast<SDL_Texture*>(sprite.tex), nullptr, &dst, rad2deg(rad), nullptr, SDL_FLIP_NONE);
}

void Renderer::drawSpriteNow(const SpriteEntry& sprite, const vec2f& pos, float rad, float scale) const {
    auto* renderer = static_cast<SDL_Renderer*>(native);
    if (!renderer) return;

    SDL_FRect dst;
    dst.x = pos.x + SCREEN.x;
    dst.y = pos.y + SCREEN.y;

	dst.w = sprite.hw * 2 * scale;
    dst.h = sprite.hh * 2 * scale;

    SDL_RenderCopyExF(renderer, static_cast<SDL_Texture*>(sprite.tex), nullptr, &dst, rad2deg(rad), nullptr, SDL_FLIP_NONE);
}

void Renderer::flush() const {
	if (vertexBuffer.empty()) return;

	SDL_RenderGeometry(
		static_cast<SDL_Renderer*>(native),
		static_cast<SDL_Texture*>(atlasTex),
		vertexBuffer.data(),
		vertexBuffer.size(),
		indexBuffer.data(),
		indexBuffer.size()
	);

	vertexBuffer.clear();
	indexBuffer.clear();
}

void Renderer::drawFilledCircle(const vec2f pos, float rad) const {
    SDL_Renderer* renderer = static_cast<SDL_Renderer*>(native);

    const int cx = static_cast<int>(pos.x) + SCREEN.x;
    const int cy = static_cast<int>(pos.y) + SCREEN.y;
    const int r  = static_cast<int>(rad);

    for(int y = -r; y <= r; y++) {
        float yy = static_cast<float>(y);
        float dx = std::sqrt(rad * rad - yy * yy);

        int ix = static_cast<int>(dx);

        SDL_RenderDrawLine(
            renderer,
            cx - ix, cy + y,
            cx + ix, cy + y
        );
    }
}

void Renderer::setColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const {
    SDL_SetRenderDrawColor(static_cast<SDL_Renderer*>(native),r,g,b,a);
}
