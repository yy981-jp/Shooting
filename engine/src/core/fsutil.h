#pragma once

#include "json.h"
#include <fstream>


inline rj::Document readJson(const std::string& path) {
    std::ifstream ifs(path);
    if (!ifs) throw std::runtime_error("readJson(): ifs");
    rj::IStreamWrapper isw(ifs);
    rj::Document doc;
    doc.ParseStream(isw);
    return doc;
}
