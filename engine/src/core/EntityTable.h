#pragma once
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <unordered_map>


class EntityTable {
public:
    uint16_t get(std::string_view name) {
        // キャッシュ命中
        auto it = cache.find(name);
        if (it != cache.end())
            return it->second;

        // 元テーブルから取得
        auto src = table.find(std::string(name));
        if (src == table.end())
            throw std::runtime_error("Unknown entity: " + std::string(name));

        uint16_t id = src->second;
        cache.emplace(src->first, id);
        return id;
    }


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

    std::unordered_map<std::string, uint16_t, StrHash, StrEq> table;

private:
    std::unordered_map<std::string_view, uint16_t, StrHash, StrEq> cache;
};
