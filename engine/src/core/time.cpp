#include "time.h"


/*------------------------------*/
/*          CheckTime           */
/*------------------------------*/

    CheckTime::CheckTime() {
		start = std::chrono::high_resolution_clock::now();
	}

    int CheckTime::end() {
		std::chrono::time_point<std::chrono::high_resolution_clock> e = std::chrono::high_resolution_clock::now();
		std::chrono::duration<long long, std::milli> duration = std::chrono::duration_cast<std::chrono::milliseconds>(e - start);
		return duration.count();
	}



/*------------------------------*/
/*          ElapsedTime         */
/*------------------------------*/

	void ElapsedTime::init(){
		start = std::chrono::high_resolution_clock::now();
		enable = true;
	}

    int ElapsedTime::get() {
		std::chrono::time_point<std::chrono::high_resolution_clock> e = std::chrono::high_resolution_clock::now();
		std::chrono::duration<long long, std::milli> duration = std::chrono::duration_cast<std::chrono::milliseconds>(e - start);
		start = std::move(e);
		return duration.count();
	}

	ElapsedTime::operator bool() {
		return enable;
	}



	void FpsCounter::update() {
		++frameCount;

		auto now = clock::now();
		std::chrono::duration<float> elapsed = now - prev;

		if (elapsed.count() >= 1.0f) {
			fps = frameCount / elapsed.count();
			frameCount = 0;
			prev = now;
		}
	}

	float FpsCounter::getFps() const {
		return fps;
	}
