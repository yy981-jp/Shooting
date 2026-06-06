#pragma once


class IntervalTrigger {
    float intervalSecond; // seconds
    float elapsedTime = 0.f;

public:
    IntervalTrigger(float intervalSecond): intervalSecond(intervalSecond) {}

    void update(float deltatime) {
        elapsedTime += deltatime;
    }
    
    /// @return spawn回数
    int get() {
        int spawnCount = static_cast<int>(elapsedTime / intervalSecond);
        elapsedTime -= spawnCount * intervalSecond;
        return spawnCount;
    }
};
