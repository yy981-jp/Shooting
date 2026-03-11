#include "text.h"
#include "util.h"

#include <SDL_image.h>
#include <SDL_ttf.h>


Text::Text(Renderer* r) {
    nativeGfx = r->getNativePtr();
	font16 = TTF_OpenFont(TTFPath.c_str(), 16);
	font32 = TTF_OpenFont(TTFPath.c_str(), 32);
	font64 = TTF_OpenFont(TTFPath.c_str(), 64);
	if (!font16 || !font32 || !font64) throw std::runtime_error(std::string("load font failed: ") + TTF_GetError());
}

SpriteEntry Text::createTextureFromTTF(const std::string& text, FontSize size, Color color) const {
    TTF_Font* targetFont;
    switch (size) {
        using enum FontSize;
        case f16: targetFont = static_cast<TTF_Font*>(font16); break;
        case f32: targetFont = static_cast<TTF_Font*>(font32); break;
        case f64: targetFont = static_cast<TTF_Font*>(font64); break;
        default: throw std::runtime_error("Text::createTextureFromTTF: switch");
    }

	SDL_Surface* s = TTF_RenderUTF8_Blended(targetFont, text.c_str(), static_cast<SDL_Color>(color));
	return createEntry(s,static_cast<SDL_Renderer*>(nativeGfx));
}

int Text::getFontLineSkip(FontSize size) const {
    TTF_Font* targetFont;
    switch (size) {
        using enum FontSize;
        case f16: targetFont = static_cast<TTF_Font*>(font16); break;
        case f32: targetFont = static_cast<TTF_Font*>(font32); break;
        case f64: targetFont = static_cast<TTF_Font*>(font64); break;
        default: throw std::runtime_error("Text::getFontLineSkip: switch");
    }
    return TTF_FontLineSkip(targetFont);
}
