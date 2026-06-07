#pragma ocne
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>


class ScoreSys {
    std::ofstream ofs;
    
    std::vector<uint64_t> scores;
    uint64_t last = 0;

    void sort() { std::sort(scores.begin(), scores.end(), std::greater<>()); }

public:
    ScoreSys(const std::string& path): ofs(path) {
        std::ifstream ifs(path);
        if (!ifs) throw std::runtime_error("failed to open file - input");
        if (!ofs) throw std::runtime_error("failed to open file - output");
        
        std::string line;
        while (std::getline(ifs,line)) {
            scores.push_back( std::stoll(line) );
        }
        ifs.close();
        sort();
    }

    uint64_t getLast() { return last; }

    const std::vector<uint64_t> get() { return scores; }

    void add(uint64_t score) {
        scores.push_back(score);
        last = score;
    }
};
