#include "BezierMover.h"

    vec2f BezierMover::calculateBezierVec2(float t) {
		std::vector<vec2f> tmp = controlVec2s;
		int n = static_cast<int>(tmp.size());

		for (int k = 1; k < n; ++k) {
			for (int i = 0; i < n - k; ++i) {
				tmp[i] = tmp[i] * (1.0f - t) + tmp[i + 1] * t;
			}
		}
		return tmp[0];
	}

	BezierMover::BezierMover(const std::span<const vec2f>& controlVec2s, int durationMs)
	 :controlVec2s(controlVec2s.begin(), controlVec2s.end()),
	  invDurationMs(durationMs > 0 ? 1.0f / static_cast<float>(durationMs) : 0.0f) {
		if (durationMs <= 0) {
			t = 1.0f;
		}
	}

	void BezierMover::update(float deltaTime) {
		if (!running) return;

		t += invDurationMs * deltaTime;
		if (t >= 1.0f) {
			t = 1.0f;
			running = false;
		}

		pos = calculateBezierVec2(t);
	}
