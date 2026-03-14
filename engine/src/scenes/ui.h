#pragma once
#include "scene.h"

#include <algorithm>


enum class CacheID {
    fps, entityNum,
    Count
};

class TextUI {
    const float hmin, hmax, wmin, wmax;
    float hcur = hmin;
    float wcur = wmin;
    FontSize currentFontSize;

    SpriteEntry* entries;

    const SceneContext& ctx;
    constexpr static AtlasID atlas = AtlasID::font;

public:
    TextUI(SceneContext& ctx, vec2f min, vec2f max)
     : ctx(ctx), hmin(min.y), wmin(min.x), hmax(max.y), wmax(max.x) {}

    Color white{255,255,255,255};

    inline void initCur() {
        hcur = hmin;
        wcur = wmin;
    }

    /// @brief write no enter, write direct string
    inline void write(FontSize size, const std::string& str) {
        currentFontSize = size;
        for (char c: str) {
            const SpriteEntry& entry = ctx.gfx->drawFont(size, c, {wcur,hcur});
            wcur += entry.hw * 2;
        }
    }

    inline void setSize(FontSize size) { currentFontSize = size; }

    inline void enter(int loop = 1) {
        const SpriteEntry& entry = ctx.gfx->getFontEntry(currentFontSize, '|'); // |は最も縦幅が長いのでそれを基準とする
        hcur += entry.hh * 2 * 2;
        wcur = wmin;
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

        for (const int& n: digits) {
            const SpriteEntry& entry = ctx.gfx->drawFont(size, '0' + n , {wcur,hcur});
            wcur += entry.hw * 2;
        }
    }

    /// @brief no enter float
    inline void write(float num, FontSize size, int significantFigures = 6) {
        currentFontSize = size;
        SpriteEntry* targetArray;
        std::string num_str = std::to_string(num);
        int loop = 0;
        for (char c: num_str) {
            loop++;
            const SpriteEntry& entry = ctx.gfx->drawFont(size, c, {wcur,hcur});
            wcur += entry.hw * 2;
            if (loop > significantFigures) return;
        }
    }
};
