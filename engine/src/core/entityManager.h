#include <vector>
#include <cstdint>
#include <limits>

using EntityID = uint32_t;

struct Slot {
    uint32_t nextFree; // 次の空きID（linked list）
    bool alive;
};





class EntityManager {
public:
    EntityManager() {
        freeHead = INVALID;
    }

    EntityID create() {
        EntityID id;

        if (freeHead != INVALID) {
            // フリースロットを再利用
            id = freeHead;
            freeHead = slots[id].nextFree;
            slots[id].alive = true;
        } 
        else {
            // 新規スロット追加
            id = (EntityID)slots.size();
            slots.push_back({INVALID, true});
        }

        return id;
    }

    void destroy(EntityID id) {
        slots[id].alive = false;

        // フリースロット linked list に追加
        slots[id].nextFree = freeHead;
        freeHead = id;
    }

    bool isAlive(EntityID id) const {
        return slots[id].alive;
    }

private:
    static constexpr uint32_t INVALID = std::numeric_limits<uint32_t>::max();

    std::vector<Slot> slots;
    uint32_t freeHead; // フリーリストの先頭
};
