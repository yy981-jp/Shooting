#include "sfx.h"

#include <stdexcept>

#include <SDL.h>
#include <SDL_mixer.h>


SFXManager::SFXManager() {
    size_t count = static_cast<size_t>(SFXID::Count);
    modes.resize(count);
    se.resize(count,nullptr);
    bgm_intro.resize(count,nullptr);
    bgm_loop.resize(count,nullptr);

    instance = this;

    #define X(mode_t, id_t) loadOne(SFXMode::mode_t, SFXID::id_t, #id_t);
    #include "../../../assets/sfx.def"
    #undef X
}

void SFXManager::play(SFXID id) {
    size_t idx = static_cast<size_t>(id);
    switch (modes[idx]) {
        case SFXMode::SE: playSE(idx); break;
        case SFXMode::BGM: {
            playBGMIntro(idx);
            currentBGM = idx;
            Mix_HookMusicFinished(playBGMLoopC);
        } break;
    }
}

void SFXManager::loadOne(SFXMode mode, SFXID id, const char* name) {
    size_t idx = static_cast<size_t>(id);

    modes[idx] = mode;

    if (mode == SFXMode::SE) {
        auto* chunk = Mix_LoadWAV((Assets + "audio/" + name + ".wav").c_str());
        if (!chunk) throw std::runtime_error("SE load failed" + std::string(Mix_GetError()));
        se[idx] = chunk;
    } else {
        auto* intro = Mix_LoadMUS((Assets + "audio/" + name + "/intro.ogg").c_str());
        if (!intro) throw std::runtime_error("BGM intro load failed" + std::string(Mix_GetError()));

        auto* loop = Mix_LoadMUS((Assets + "audio/" + name + "/loop.ogg").c_str());
        if (!loop) throw std::runtime_error("BGM loop load failed" + std::string(Mix_GetError()));

        bgm_intro[idx] = intro;
        bgm_loop[idx] = loop;
    }
}

void SFXManager::playSE(size_t idx) {
    if ( Mix_PlayChannel(-1,static_cast<Mix_Chunk*>(se[idx]),0) < 0 ) throw std::runtime_error("SFX::playSE");
}

void SFXManager::playBGMIntro(size_t idx) {
    Mix_PlayMusic(static_cast<Mix_Music*>(bgm_intro[idx]),0);
}

void SFXManager::playBGMLoop(size_t idx) {
    Mix_PlayMusic(static_cast<Mix_Music*>(bgm_loop[idx]),-1);
}

void SFXManager::playBGMLoopC() {
    Mix_PlayMusic(static_cast<Mix_Music*>(instance->bgm_loop[instance->currentBGM]),-1);
}
