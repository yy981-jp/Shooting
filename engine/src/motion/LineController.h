#pragma once
#include "../core/cache.h"
#include "../core/def.h"


struct LineController {
    float m_angle;
    float m_speed;

    LineController(float angle, float speed):
        m_angle(angle), m_speed(speed) {}

    void update(float dt, MotionState& ms) {
        ms.angle = m_angle;
        ms.speed = m_speed;
    }
};
