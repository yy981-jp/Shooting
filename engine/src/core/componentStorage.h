#pragma once
#include <vector>
#include <cassert>

#include "../core/def.h"

struct IStorage {
    virtual void remove(EntityHandle) = 0;
};


template<typename T>
class ComponentStorage: public IStorage {
public:
    // entity id -> dense index
    std::vector<EntityID> sparse;

    // dense storage
    std::vector<EntityID> denseEntities;
    std::vector<T> denseData;

public:
    void reserve(size_t maxEntities) {
        sparse.resize(maxEntities, UINT32_MAX);
    }

    bool has(EntityHandle e) const {
        return e.id < sparse.size() && sparse[e.id] != UINT32_MAX;
    }

    T& get(EntityHandle e) {
        assert(has(e));
        return denseData[sparse[e.id]];
    }

    const T& get(EntityHandle e) const {
        assert(has(e));
        return denseData[sparse[e.id]];
    }

    void add(EntityHandle e, const T& value = T{}) {
        assert(!has(e));
        EntityID index = (EntityID)denseData.size();
        denseData.push_back(value);
        denseEntities.push_back(e.id);
        sparse[e.id] = index;
    }

    void remove(EntityHandle e) override {
        assert(has(e));
        EntityID index = sparse[e.id];
        EntityID last = (EntityID)denseData.size() - 1;

        // swap with last
        denseData[index] = denseData[last];
        denseEntities[index] = denseEntities[last];

        // update sparse for moved entity
        sparse[denseEntities[index]] = index;

        denseData.pop_back();
        denseEntities.pop_back();
        sparse[e.id] = UINT32_MAX;
    }

    size_t size() const {
        return denseData.size();
    }

    // iteration
    T& operator[](size_t i) { return denseData[i]; }
    const T& operator[](size_t i) const { return denseData[i]; }
/*
    EntityHandle entityAt(size_t i) const {
        return EntityHandle{ denseEntities[i], 0 }; gen=0固定は良くないのでは? 
                                                    意味を持たないことを表す数値を追加するとか
    }
*/
};
