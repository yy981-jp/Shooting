#include "entityManager.h"

    EntityManager::EntityManager() {
        freeHead = INVALID;
    }

    EntityHandle EntityManager::create() {
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

    void EntityManager::destroy(EntityHandle h) {
        ++records[h.id].gen;

        // フリースロット linked list に追加
        records[h.id].nextFree = freeHead;
        freeHead = h.id;
    }

    bool EntityManager::is_alive(EntityHandle h) {
        return records[h.id].gen == h.gen;
    }



EntityManager entMgr;