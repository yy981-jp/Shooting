#pragma once

#include <vector>
#include <span>
#include "../core/def.h"

class BezierController {
	vec2f origin;
	std::vector<vec2f> controlVec2s, work;
	float t = 0.0f;
	float invDurationMs;
	bool running = true;

	vec2f calculateBezierVec2(float t);

public:
	BezierController(std::span<const vec2f> controlVec2s, int durationMs, vec2f startPos);

	void update(float deltaTime, vec2f& pos, vec2f& vel);
	bool isRunning() const { return running; }
};
