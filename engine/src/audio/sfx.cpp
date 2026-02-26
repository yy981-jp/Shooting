#include "sfx.h"

#include <stdexcept>

#include <SDL.h>
#include <SDL_mixer.h>


SFX::SFX(SFXMode i_mode, std::string name): mode(i_mode) {
    switch (mode) {
        case SFXMode::se: {
            Mix_Chunk* se = Mix_LoadWAV((Assets + "audio/" + name + ".wav").c_str());
            if (!se) throw std::runtime_error("SFX::SFX: se is nullptr");
            ptr1 = se;
        } break;
        case SFXMode::bgm: {
            Mix_Music* intro = Mix_LoadMUS((Assets + "audio/" + name + "intro.ogg").c_str());
            if (!intro) throw std::runtime_error("SFX::SFX: bgm.intro is nullptr");
            Mix_Music* loop = Mix_LoadMUS((Assets + "audio/" + name + "loop.ogg").c_str());
            if (!loop) throw std::runtime_error("SFX::SFX: bgm.loop is nullptr");
            ptr1 = intro;
            ptr2 = loop;
        }
    }
}

void SFX::playSE() {
    if (mode != SFXMode::se) throw std::runtime_error("SFX::playSE: isn't se");
    if ( Mix_PlayChannel(-1,static_cast<Mix_Chunk*>(ptr1),0) < 0 ) throw std::runtime_error("SFX::playSE");
}

void SFX::playBGMIntro() {
    if (mode != SFXMode::bgm) throw std::runtime_error("SFX::playBGMIntro");
    Mix_PlayMusic(static_cast<Mix_Music*>(ptr1),0);
}

void SFX::playBGMLoop() {
    if (mode != SFXMode::bgm) throw std::runtime_error("SFX::playBGMLoop");
    Mix_PlayMusic(static_cast<Mix_Music*>(ptr2),-1);
}
