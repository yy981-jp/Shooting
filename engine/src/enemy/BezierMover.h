#pragma once

#include <vector>
#include <span>

#include "../core/def.h"

class BezierMover {
	std::vector<vec2f> controlVec2s;
	float t = 0.0f;
	float invDurationMs;
	bool running = true;

	vec2f calculateBezierVec2(float t);
	
public:
	vec2f pos;

	// duration is expected to be in milliseconds
	BezierMover(const std::span<const vec2f>& controlVec2s, int durationMs);
	void update(int deltaTime);
	bool isRunning() const {
		return running;
	}
};
