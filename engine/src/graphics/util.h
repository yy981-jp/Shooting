#pragma once
#include "gfx.h"

#include <SDL_image.h>


inline SpriteEntry createEntry(SDL_Surface* s, SDL_Renderer* renderer) {
    if (!s) throw std::runtime_error(std::string("IMG_Load error: ") + IMG_GetError());

    int width  = s->w;
    int height = s->h;

    SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
    if (!t) throw std::runtime_error(std::string("SDL_CreateTextureFromSurface") + IMG_GetError());
	SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND); // 透過有効
    
	SDL_FreeSurface(s);

    return SpriteEntry{static_cast<void*>(t), width/2, height/2};
}
