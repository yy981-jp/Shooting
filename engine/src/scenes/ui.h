#pragma once
#include "scene.h"

#include <cstring>


enum class CacheID {
    fps, entityNum,
    Count
};

class TextUI {
    const float hmin = -390;
    const float hmax = 390;
    float hcur = hmin;
    const float wmin = 410;
    const float wmax = 600;
    float wcur = wmin;
    FontSize currentFontSize;

    SpriteEntry* entries;

    const SceneContext& ctx;
    constexpr static AtlasID atlas = AtlasID::font;

public:
    TextUI(SceneContext& ctx, vec2f min, vec2f max)
     : ctx(ctx), hmin(min.y), wmin(min.x), hmax(max.y), wmax(max.x) {

    }

    Color white{255,255,255,255};

    inline void initCur() {
        hcur = hmin;
        wcur = wmin;
    }

    /// @brief write no enter, write direct string
    inline void write(FontSize size, const std::string& str) {
        currentFontSize = size;
        const auto entry = ctx.txtgfx->createTextureFromTTF(str, size, white);
        ctx.gfx->drawSprite(, {wcur,hcur});
        wcur += entry.hw * 2;
    }

    inline void setSize(FontSize size) { currentFontSize = size; }

    inline void enter(int loop = 1) {
        hcur += ctx.txtgfx->getFontLineSkip(currentFontSize) * loop;
        wcur = wmin;
    }

    /// @brief write no enter (use cache)
    inline void write(CacheID id) {
        currentFontSize = cacheIndex[static_cast<size_t>(id)];
        const auto& entry = cache[static_cast<size_t>(id)];
        ctx.gfx->drawSprite(entry, {wcur,hcur});
        wcur += entry.hw * 2;
    }

    /// @brief no enter int
    inline void write(int num, FontSize size) {
        currentFontSize = size;

        std::vector<int> digits;
        while (num > 0) {
            digits.push_back(num % 10);
            num /= 10;
        }
        std::reverse(digits.begin(), digits.end());

        SpriteEntry* targetArray;
        switch (size) {
            using enum FontSize;
            case f16: targetArray = numCache16; break;
            case f32: targetArray = numCache32; break;
            case f64: targetArray = numCache64; break;
        }
        for (const int& n: digits) {
            const SpriteEntry& entry = targetArray[n];
            ctx.gfx->drawSpriteNow(entry, {wcur,hcur});
            wcur += entry.hw * 2;
        }
    }

    /// @brief no enter float
    inline void write(float num, FontSize size, int significantFigures = 6) {
        currentFontSize = size;
        SpriteEntry* targetArray;
        std::string num_str = std::to_string(num);
        switch (size) {
            using enum FontSize;
            case f16: targetArray = numCache16; break;
            case f32: targetArray = numCache32; break;
            case f64: targetArray = numCache64; break;
        }
        int loop = 0;
        for (char n: num_str) {
            loop++;
            if (loop > significantFigures) return;
            int index;
            if (n == '.') index = 10;
            else index = n - '0';
            const SpriteEntry& entry = targetArray[index];
            ctx.gfx->drawSprite(entry, {wcur,hcur});
            wcur += entry.hw * 2;
        }
    }
};
