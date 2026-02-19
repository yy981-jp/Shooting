#pragma once


class spawnManager {
    int shootInterval; // ms
    float elapsedTime = 0.0f;

public:
    spawnManager(int shootInterval): shootInterval(shootInterval) {}

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
