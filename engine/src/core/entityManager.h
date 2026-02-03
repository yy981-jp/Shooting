#pragma once

#include <vector>
#include <cstdint>
#include <limits>

#include "def.h"



class EntityManager {
    struct Slot {
        uint32_t nextFree; // 次の空きID（linked list）
        EntityGen gen = 0; // 世代
        uint64_t mask;     // component mask (bitflagでEntityが所属するcomponent達を表す)
    };

public:
    EntityManager() {
        freeHead = INVALID;
    }

    EntityHandle create() {
        EntityID id;

        if (freeHead != INVALID) {
            // フリースロットを再利用
            id = freeHead;
            freeHead = records[id].nextFree;
        } else {
            // 新規スロット追加
            id = (EntityID)records.size();
            records.push_back({INVALID});
        }

        return {id,records[id].gen};
    }

    void destroy(EntityHandle h) {
        ++records[h.id].gen;

        // フリースロット linked list に追加
        records[h.id].nextFree = freeHead;
        freeHead = h.id;
    }

    bool is_alive(EntityHandle h) {
        return records[h.id].gen == h.gen;
    }

    uint64_t getMask(EntityHandle e) const {
        if (e.id >= records.size()) return 0;
        const auto& r = records[e.id];
        if (r.gen != e.gen) return 0; // dead
        return r.mask;
    }

    bool hasComponent(EntityHandle e, uint64_t mask) const { // 少なくとも要求maskをentityが持っていればtrue
        return (getMask(e) & mask) == mask;
    }

private:
    static constexpr uint32_t INVALID = std::numeric_limits<uint32_t>::max();

    std::vector<Slot> records;
    uint32_t freeHead; // フリーリストの先頭
} entMgr;
