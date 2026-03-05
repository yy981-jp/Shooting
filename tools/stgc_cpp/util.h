#pragma once
#include <vector>
#include <string>

std::vector<std::string> splitlines(const std::string& s) {
    std::vector<std::string> result;
    std::string current;

    for (char c : s) {
        if (c == '\n') {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }

    if (!current.empty())
        result.push_back(current);

    return result;
}

std::string strip(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace((unsigned char)s[start]))
        ++start;

    size_t end = s.size();
    while (end > start && std::isspace((unsigned char)s[end - 1]))
        --end;

    return s.substr(start, end - start);
}
