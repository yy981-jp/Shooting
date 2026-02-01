#pragma once

#include <chrono>


class CheckTime {
public:
    CheckTime();
    int end();
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

class ElapsedTime {
public:
    ElapsedTime() {}
	void init();
    int get();
	operator bool();
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	bool enable = false;
};



class FpsCounter {
	using clock = std::chrono::steady_clock;

	clock::time_point prev = clock::now();
	int frameCount = 0;
	float fps = 0.0f;

public:
	void update();
	float getFps() const;
};
