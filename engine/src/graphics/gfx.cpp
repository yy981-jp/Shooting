#include "gfx.h"
#include "../core/mathUtil.h"
#include "../core/cache.h"

#include <SDL.h>
#include <SDL_image.h>
#include <stdexcept>
#include <cmath>


SpriteInfo loadSprite(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* s = IMG_Load(path.c_str());
    if (!s) {
        throw std::runtime_error(std::string("IMG_Load error: ") + IMG_GetError());
    }

    int width  = s->w;
    int height = s->h;

    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    if (!t) throw std::runtime_error(std::string("SDL_CreateTextureFromSurface") + IMG_GetError());

    SDL_FreeSurface(s);

    return SpriteInfo{static_cast<void*>(t), width/2, height/2};
}


/*------------------------------**
**          Renderer            **
**------------------------------*/
Renderer::Renderer(void* sdlRenderer, int halfWidth, int halfHeight): native(sdlRenderer) {
    auto* renderer = static_cast<SDL_Renderer*>(native);

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

vec2i Renderer::getSpriteHalfSize(SpriteID spriteID) const {
    const SpriteInfo& sprite = spriteTable[static_cast<size_t>(spriteID)];
    vec2i vec;
    vec.x = sprite.hw;
    vec.y = sprite.hh;
    return vec;
}

vec2i Renderer::getSpriteSize(SpriteID spriteID) const {
    return getSpriteHalfSize(spriteID) * 2;
}

void Renderer::drawSprite(SpriteID spriteID, const vec2f& pos, float rad) const {
	SDL_Texture* tex = static_cast<SDL_Texture*>(
		spriteTable[static_cast<size_t>(spriteID)].tex
	);

	if (currentTexture && currentTexture != tex)
		throw std::runtime_error("gfx::drawSprite: currentTexture != tex");

	currentTexture = static_cast<void*>(tex);

	const auto& sp = spriteTable[static_cast<size_t>(spriteID)];

	float hw = sp.hw;
	float hh = sp.hh;

	int baseIndex = vertexBuffer.size();

	// ---- 回転なしルート ----
	if (rad == 0.0f)
	{
		SDL_Vertex v[4];

		v[0].position = { SCREEN.x + pos.x - hw, SCREEN.y + pos.y - hh };
		v[1].position = { SCREEN.x + pos.x + hw, SCREEN.y + pos.y - hh };
		v[2].position = { SCREEN.x + pos.x + hw, SCREEN.y + pos.y + hh };
		v[3].position = { SCREEN.x + pos.x - hw, SCREEN.y + pos.y + hh };

		for (int i = 0; i < 4; ++i)
		{
			v[i].color = {255,255,255,255};

			v[i].tex_coord = {
				(i == 1 || i == 2) ? 1.0f : 0.0f,
				(i >= 2) ? 1.0f : 0.0f
			};

			vertexBuffer.push_back(v[i]);
		}
	}
	// ---- 回転ありルート ----
	else
	{
		float c = cachesv.getCos(rad);
		float s = -cachesv.getSin(rad);

		SDL_FPoint corners[4] = {
			{-hw, -hh},
			{ hw, -hh},
			{ hw,  hh},
			{-hw,  hh}
		};

		for (int i = 0; i < 4; ++i)
		{
			float rx = corners[i].x * c - corners[i].y * s;
			float ry = corners[i].x * s + corners[i].y * c;

			SDL_Vertex v;

			v.position = {
				SCREEN.x + pos.x + rx,
				SCREEN.y + pos.y + ry
			};

			v.color = {255,255,255,255};

			v.tex_coord = {
				(i == 1 || i == 2) ? 1.0f : 0.0f,
				(i >= 2) ? 1.0f : 0.0f
			};

			vertexBuffer.push_back(v);
		}
	}

	indexBuffer.push_back(baseIndex + 0);
	indexBuffer.push_back(baseIndex + 1);
	indexBuffer.push_back(baseIndex + 2);
	indexBuffer.push_back(baseIndex + 2);
	indexBuffer.push_back(baseIndex + 3);
	indexBuffer.push_back(baseIndex + 0);
}

void Renderer::drawSpriteNow(SpriteID spriteID, const vec2f& pos, float rad) const {
    auto* renderer = static_cast<SDL_Renderer*>(native);
    if (!renderer) return;

    SpriteInfo sprite = spriteTable[static_cast<size_t>(spriteID)];

    SDL_Rect dst;
    dst.x = pos.x + SCREEN.x - sprite.hw;
    dst.y = pos.y + SCREEN.y - sprite.hh;
    dst.w = sprite.hw * 2;
    dst.h = sprite.hh * 2;

    SDL_RenderCopyEx(renderer, static_cast<SDL_Texture*>(sprite.tex), nullptr, &dst, rad2deg(rad), nullptr, {});
}

void Renderer::flush() const {
	if (vertexBuffer.empty() || !currentTexture) return;

	SDL_RenderGeometry(
		static_cast<SDL_Renderer*>(native),
		static_cast<SDL_Texture*>(currentTexture),
		vertexBuffer.data(),
		vertexBuffer.size(),
		indexBuffer.data(),
		indexBuffer.size()
	);

	vertexBuffer.clear();
	indexBuffer.clear();
    currentTexture = nullptr;
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
