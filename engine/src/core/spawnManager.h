#pragma once


class spawnManager {
    int shootInterval; // ms
    int elapsedTime = 0;

public:
    spawnManager(int shootInterval): shootInterval(shootInterval) {}

    void update(int deltatime) {
        elapsedTime += deltatime;
    }
    
    /// @return spawn回数
    int get() {
        int spawnCount = elapsedTime / shootInterval;
        elapsedTime %= shootInterval;
        return spawnCount;
    }
};
