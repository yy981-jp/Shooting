#pragma once


class spawnManager {
    float shootInterval; // seconds
    float elapsedTime = 0.0f;

public:
    spawnManager(int shootIntervalMs): shootInterval(shootIntervalMs / 1000.0f) {}

    void update(float deltatime) {
        elapsedTime += deltatime;
    }
    
    /// @return spawn回数
    int get() {
        int spawnCount = static_cast<int>(elapsedTime / shootInterval);
        elapsedTime -= spawnCount * shootInterval;
        return spawnCount;
    }
};
