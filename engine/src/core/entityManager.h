#pragma once

#include <vector>
#include <cstdint>
#include <limits>

#include "def.h"



class EntityManager {
    struct Entry {
        uint32_t nextFree; // 次の空きID（linked list）
        EntityGen gen = 0; // 世代
        void* ptr;
    };
    uint32_t aliveCount = 0;

public:
    EntityManager();
    EntityHandle create();
    void destroy(EntityHandle h);
    bool is_alive(EntityHandle h) const;
    void setPtr(EntityHandle e, void* ptr);
    uint32_t size() const;

    template<typename T>
    T* getPtr(const EntityHandle& h) const {
        if (!is_alive(h)) return nullptr;
        return static_cast<T*>(records[h.id].ptr);
    }

private:
    static constexpr uint32_t INVALID = std::numeric_limits<uint32_t>::max();

    std::vector<Entry> records;
    uint32_t freeHead; // フリーリストの先頭
};


extern EntityManager entMgr;
