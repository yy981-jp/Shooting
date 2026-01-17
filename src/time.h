#pragma once

#include <chrono>


class checkTime {
public:
    checkTime() {
		start = std::chrono::high_resolution_clock::now();
	}

    int end() {
		std::chrono::time_point<std::chrono::high_resolution_clock> e = std::chrono::high_resolution_clock::now();
		std::chrono::duration<long long, std::milli> duration = std::chrono::duration_cast<std::chrono::milliseconds>(e - start);
		return duration.count();
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
};

class ElapsedTime {
public:
    ElapsedTime() {}

	void init(){
		start = std::chrono::high_resolution_clock::now();
		enable = true;
	}

    int get() {
		std::chrono::time_point<std::chrono::high_resolution_clock> e = std::chrono::high_resolution_clock::now();
		std::chrono::duration<long long, std::milli> duration = std::chrono::duration_cast<std::chrono::milliseconds>(e - start);
		start = std::move(e);
		return duration.count();
	}

	operator bool() {
		return enable;
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	bool enable = false;
};
