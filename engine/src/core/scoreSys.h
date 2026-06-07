#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <span>


class ScoreSys {
    std::string path;
    
    std::vector<uint64_t> scores;
    uint64_t last = 0;

    void sort();

public:
    ScoreSys(const std::string& path);
    
    uint64_t getLast();
    std::span<const uint64_t> get(int maxSize = -1);
    void add(uint64_t score);
    
    void save();
};
