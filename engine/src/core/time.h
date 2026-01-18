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
