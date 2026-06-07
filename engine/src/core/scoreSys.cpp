#include "scoreSys.h"
#include <filesystem>

namespace fs = std::filesystem;


void ScoreSys::sort() { std::sort(scores.begin(), scores.end(), std::greater<>()); }

ScoreSys::ScoreSys(const std::string& path): path(path) {
    // load
    std::ifstream ifs(path);
    if (!ifs) return; // 初回起動
    
    std::string line;
    while (std::getline(ifs,line)) {
        scores.push_back( std::stoull(line) );
    }
    sort();
}

uint64_t ScoreSys::getLast() { return last; }

std::span<const uint64_t> ScoreSys::get(int maxSize) {
    if (maxSize == -1) {
        return {
            scores.data(),
            scores.size()
        };
    } else {
        return {
            scores.data(),
            std::min<size_t>(maxSize, scores.size())
        };
    }
}

void ScoreSys::add(uint64_t score) {
    last = score;

    auto pos = std::lower_bound(
        scores.begin(),
        scores.end(),
        score,
        std::greater<>()
    );

    scores.insert(pos, score);

    save();
}

void ScoreSys::save() {
    std::ofstream ofs(path);
    if (!ofs) throw std::runtime_error("failed to open file - output");
    
    for (uint64_t score: scores) {
        ofs << score << "\n";
    }
}
