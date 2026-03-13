#include "gfx.h"
#include "util.h"
#include "text.h"
#include "../core/fsutil.h"
#include "../core/mathUtil.h"
#include "../core/cache.h"

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdexcept>
#include <cmath>


constexpr std::array<std::string,
    static_cast<size_t>(SpriteID::Count)> entityNames = {
#define X(name) #name,
#include "../../../assets/sprite.def"
#undef X
};

Color::operator SDL_Color() const {
    return SDL_Color{r,g,b,a};
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


void Renderer::loadSpriteAtlas(const AtlasID id, const std::string atlasName) {
    auto* renderer = static_cast<SDL_Renderer*>(native);

    SDL_Surface* spriteAtlas = IMG_Load((Assets + atlasName + ".png").c_str());
	if (!spriteAtlas) throw std::runtime_error("gfx: couldn't open sprite texture");

	int tex_w = spriteAtlas->w;
	int tex_h = spriteAtlas->h;
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, spriteAtlas);
	SDL_FreeSurface(spriteAtlas);

	atlasTex[static_cast<size_t>(id)] = tex;
	
	auto&& spriteAtlasTable = readJson(Assets + atlasName + ".json");

	// check json
	if (spriteAtlasTable.HasMember("version")) {
		if (spriteAtlasTable["version"].GetInt() != 1) throw std::runtime_error("gfx: atlasjson: mismatch version");
	} else throw std::runtime_error("gfx: sprite-atlasjson: didn't contain version");
	
	// sprite 読み込み
    for (size_t i = 0; i < entityNames.size(); ++i) {
        auto&& name = entityNames[i];

		const auto& spriteArray = spriteAtlasTable[name.c_str()]["frame"].GetArray();
		int frames = spriteArray.Size();
		Sprite sprite;
		sprite.entries = new SpriteEntry[frames];
		sprite.frameCount = frames;
		if (spriteAtlasTable[name.c_str()].HasMember("spf"))
			sprite.spf = spriteAtlasTable[name.c_str()]["spf"].GetFloat();

		// load sprite
		int frameIndex = 0;
		for (const rapidjson::Value& spriteFrame: spriteArray) {
			// load sprite frame
			int x = spriteFrame[0].GetInt();
			int y = spriteFrame[1].GetInt();
			int w = spriteFrame[2].GetInt();
			int h = spriteFrame[3].GetInt();

			// entry 構築
			SpriteEntry entry;
			entry.u1 = (float)x / tex_w;
			entry.v1 = (float)y / tex_h;
			entry.u2 = (float)(x + w) / tex_w;
			entry.v2 = (float)(y + h) / tex_h;
			entry.hw = w / 2;
			entry.hh = h / 2;
			entry.id = AtlasID::sprite;

			// spriteに格納
			sprite.entries[frameIndex] = entry;
			frameIndex++;
		}

		spriteTable[i] = sprite;
    }
}

void Renderer::loadFontAtlas(const AtlasID id, const std::string atlasName) {
    auto* renderer = static_cast<SDL_Renderer*>(native);

    SDL_Surface* atlas = IMG_Load((Assets + atlasName + ".png").c_str());
	if (!atlas) throw std::runtime_error("gfx: couldn't open font texture");

	int tex_w = atlas->w;
	int tex_h = atlas->h;
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, atlas);
	SDL_FreeSurface(atlas);

	atlasTex[static_cast<size_t>(id)] = tex;
	
	auto&& atlasTable = readJson(Assets + atlasName + ".json");

	// check json
	if (atlasTable.HasMember("version")) {
		if (atlasTable["version"].GetInt() != 1) throw std::runtime_error("gfx: atlasjson: mismatch version");
	} else throw std::runtime_error("gfx: font-atlasjson: didn't contain version");
	
	// load font entry
	int frameIndex = 0;
	for (int i = 0; i < static_cast<int>(FontSize::Count); i++) {
		rj::Value& sizeEntry = atlasTable[fontSizeName[i].c_str()];
		for (int c = ' '; c < '~'; c++) { // 32 ~ 126 -> 95
			int index_insize = c - ' ';
			rj::Value& entry_j = sizeEntry[std::to_string(c).c_str()];

			int x = entry_j[0].GetInt();
			int y = entry_j[1].GetInt();
			int w = entry_j[2].GetInt();
			int h = entry_j[3].GetInt();

			// entry 構築
			SpriteEntry entry;
			entry.u1 = (float)x / tex_w;
			entry.v1 = (float)y / tex_h;
			entry.u2 = (float)(x + w) / tex_w;
			entry.v2 = (float)(y + h) / tex_h;
			entry.hw = w / 2;
			entry.hh = h / 2;
			entry.id = AtlasID::font;

			int index = i * ASCII_RANGE + index_insize;
			fontTable[index] = entry;
		}
	}
}


Renderer::Renderer(void* sdlRenderer, int halfWidth, int halfHeight): native(sdlRenderer) {
	loadSpriteAtlas(AtlasID::sprite, "atlas");
	loadFontAtlas(AtlasID::font, "font");
}

Renderer::~Renderer() {
    for (size_t i = 0; i < static_cast<int>(AtlasID::Count); ++i)
        SDL_DestroyTexture(static_cast<SDL_Texture*>(atlasTex[i]));
}

vec2f Renderer::getSpriteHalfSize(SpriteID spriteID) const {
    const SpriteEntry& sprite = spriteTable[static_cast<size_t>(spriteID)][0];
    vec2f vec;
    vec.x = sprite.hw;
    vec.y = sprite.hh;
    return vec;
}

vec2f Renderer::getSpriteSize(SpriteID spriteID) const {
    return getSpriteHalfSize(spriteID) * 2;
}

const Sprite& Renderer::getSprite(SpriteID spriteid) const {
	return spriteTable[static_cast<size_t>(spriteid)];
}


void Renderer::drawSprite(SpriteID spriteID, const vec2f& pos, float rad, uint16_t frameIndex) const {
	const Sprite& sp = spriteTable[static_cast<size_t>(spriteID)];
	const SpriteEntry& ent = sp[frameIndex];

	if (currentAtlas == AtlasID::null) currentAtlas = ent.id;

	if (ent.id != currentAtlas) flush();

	float hw = ent.hw;
	float hh = ent.hh;

	float u1 = ent.u1;
	float v1 = ent.v1;
	float u2 = ent.u2;
	float v2 = ent.v2;

	int baseIndex = vertexBuffer.size();

	if (rad == 0.0f) {
		// ---- 回転なし ----
		SDL_Vertex v[4];

		vec2f graPos = SCREEN + pos;

		v[0].position = { graPos.x - hw, graPos.y - hh };
		v[1].position = { graPos.x + hw, graPos.y - hh };
		v[2].position = { graPos.x + hw, graPos.y + hh };
		v[3].position = { graPos.x - hw, graPos.y + hh };

		v[0].tex_coord = { u1, v1 };
		v[1].tex_coord = { u2, v1 };
		v[2].tex_coord = { u2, v2 };
		v[3].tex_coord = { u1, v2 };

		for (int i = 0; i < 4; ++i) {
			v[i].color = {255,255,255,255};
			vertexBuffer.push_back(v[i]);
		}
	} else {
		// ---- 回転あり ----
		float c = cachesv.getCos(rad);
		float s = cachesv.getSin(rad);

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

void Renderer::flush() const {
	if (vertexBuffer.empty()) return;

	SDL_RenderGeometry(
		static_cast<SDL_Renderer*>(native),
		static_cast<SDL_Texture*>(atlasTex[static_cast<size_t>(currentAtlas)]),
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
