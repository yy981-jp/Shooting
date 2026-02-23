#pragma once

#include "../core/def.h"
#include "../core/cache.h"


class WaveDecorator {
    float amplitude;
    float frequency;
    float time = 0.0f;

public:
    WaveDecorator(float amp,float freq)
      :amplitude(amp), frequency(freq) {}

    void update(float dt, vec2f& pos, vec2f& vel) {
        time += dt;
        vel.x += amplitude * cachesv.getSin(frequency * time);
    }
};
