#pragma once
#include <string_view>

struct StrHash {
    using is_transparent = void;
    
    size_t operator()(std::string_view s) const noexcept {
        return std::hash<std::string_view>{}(s);
    }
};

struct StrEq {
    using is_transparent = void;
    
    bool operator()(std::string_view a, std::string_view b) const noexcept {
        return a == b;
    }
};
