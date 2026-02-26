#pragma once
#include "../core/def.h"


enum class SFXMode {
    bgm, se
};

class SFX {
    SFXMode mode;

    /// @details SE ptr1 = se, ptr2 = null
    /// @details BGM ptr1 = intro, ptr2 = loop
    void *ptr1, *ptr2;

public:
    SFX(SFXMode i_mode, std::string name);

    void playSE();
    void playBGMIntro();
    void playBGMLoop();
};
