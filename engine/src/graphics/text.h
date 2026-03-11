#pragma once
#include "gfx.h"


enum class FontSize {
    f16, f32, f64
};

class Text {
    void* nativeGfx;
    void *font16, *font32, *font64;

    const std::string TTFPath = Assets + "NotoSansJP.ttf";

public:
    Text(Renderer* r);
    SpriteEntry createTextureFromTTF(const std::string& text, FontSize size, Color color) const;
    int getFontLineSkip() const;
    int getFontLineSkip(FontSize size) const;
};
