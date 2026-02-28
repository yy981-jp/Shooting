#pragma once
#include "../core/def.h"

#include <vector>


enum class SFXID : uint16_t {
#define X(mode, id) id,
	#include "../../../assets/sfx.def"
#undef X
	Count
};

enum class SFXMode: uint8_t {
    BGM, SE
};

class SFXManager {
    std::vector<bool> phase2;
    std::vector<SFXMode> modes;
    std::vector<void*> se;
    std::vector<void*> bgm_intro;
    std::vector<void*> bgm_loop;

public:
    SFXManager();
    void play(SFXID id);
private:
    void loadOne(SFXMode mode, SFXID id, const char* name);
    void playSE(size_t idx);
    void playBGMIntro(size_t idx);
    void playBGMLoop(size_t idx);
};
