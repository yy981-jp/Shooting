#pragma once

#include <vector>

#include "../core/def.h"

class BezierMover {
	std::vector<vec2f> controlVec2s;
	float t = 0.0f;
	float speedStep;
	bool running = true;

	vec2f calculateBezierVec2(float t) {
		std::vector<vec2f> tmp = controlVec2s;
		int n = static_cast<int>(tmp.size());

		for (int k = 1; k < n; ++k) {
			for (int i = 0; i < n - k; ++i) {
				tmp[i] = tmp[i] * (1.0f - t) + tmp[i + 1] * t;
			}
		}
		return tmp[0];
	}

public:
	vec2f pos;

	BezierMover(const std::span<const vec2f>& controlVec2s, int duration = 5000)
		: controlVec2s(controlVec2s.begin(), controlVec2s.end()),
		  speedStep(1.0f / static_cast<float>(duration)) {}

	void update(int deltaTime) {
		if (!running) return;

		t += speedStep * static_cast<float>(deltaTime);
		if (t >= 1.0f) {
			t = 1.0f;
			running = false;
		}

		pos = calculateBezierVec2(t);
	}

	bool isRunning() const {
		return running;
	}
};
