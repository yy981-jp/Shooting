#pragma once
#include "scene.h"

#include <cstring>


enum class CacheID {
    fps, entityNum,
    Count
};

class PlaySceneUI {
    static constexpr float min = -390;
    static constexpr float max = 390;
    float cur = min;
    static constexpr float wmin = 410;
    static constexpr float wmax = 600;
    float wcur = wmin;
    FontSize currentFontSize;

    const SceneContext& ctx;

public:
    PlaySceneUI(SceneContext& ctx): ctx(ctx) {}

    Color white{255,255,255,255};

    inline void initTex(CacheID id, FontSize size, const std::string str) {
        cache[static_cast<size_t>(id)] = ctx.txtgfx->createTextureFromTTF(str, size, white);
    }

    inline void initCur() {
        cur = min;
        wcur = wmin;
    }

    /// @brief write no enter, write direct string
    inline void write(FontSize size, const std::string& str) {
        currentFontSize = size;
        const auto entry = ctx.txtgfx->createTextureFromTTF(str, size, white);
        ctx.gfx->drawSpriteNow(entry, {wcur,cur});
        wcur += entry.hw * 2;
    }

    inline void setSize(FontSize size) { currentFontSize = size; }

    inline void enter(int loop = 1) {
        cur += ctx.txtgfx->getFontLineSkip(currentFontSize) * loop;
        wcur = wmin;
    }

    /// @brief write no enter (use cache)
    inline void write(CacheID id) {
        currentFontSize = cacheIndex[static_cast<size_t>(id)];
        const auto& entry = cache[static_cast<size_t>(id)];
        ctx.gfx->drawSpriteNow(entry, {wcur,cur});
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
            ctx.gfx->drawSpriteNow(entry, {wcur,cur});
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
            ctx.gfx->drawSprite(entry, {wcur,cur});
            wcur += entry.hw * 2;
        }
    }
};
