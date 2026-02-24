#pragma once

#include "../core/def.h"
#include "../core/cache.h"


struct WaveDecorator {
    float amplitude;  // rad
    float frequency;  // 振動速度
    float time = 0.0f;

    WaveDecorator(float amp, float freq)
        : amplitude(amp), frequency(freq) {}

    void update(float dt, MotionState& ms) {
        time += dt;

        float wave = amplitude * cachesv.getSin(frequency * time);

        ms.angle += wave;
    }
};
