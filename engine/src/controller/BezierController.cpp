#include "BezierController.h"

vec2f BezierController::calculateBezierVec2(float t) {
	int n = static_cast<int>(work.size());
	std::copy(controlVec2s.begin(), controlVec2s.end(), work.begin());

	for (int k = 1; k < n; ++k) 
		for (int i = 0; i < n - k; ++i) 
			work[i] = work[i] * (1.0f - t) + work[i + 1] * t;
	
	return work[0];
}

BezierController::BezierController(std::span<const vec2f> controlVec2s, int durationMs, vec2f startPos)
	: controlVec2s(controlVec2s.begin(), controlVec2s.end()),
	  invDurationMs(durationMs > 0 ? 1000.0f / static_cast<float>(durationMs) : 0.0f),
	  origin(startPos - controlVec2s[0]) {
	if (durationMs <= 0)
		t = 1.0f;
	work.resize(controlVec2s.size());
}

void BezierController::update(float deltatime, MotionState& ms) {
	if (!running) return;

	t += invDurationMs * deltatime;
	if (t >= 1.0f) {
		t = 1.0f;
		running = false;
	}

	vec2f target = calculateBezierVec2(t) + origin;
	// vel = target - pos;
	ms.pos = target;
}
