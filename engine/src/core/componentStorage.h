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
    std::vector<uint32_t> sparse;

    // dense storage
    std::vector<uint32_t> denseEntities;
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
        uint32_t index = (uint32_t)denseData.size();
        denseData.push_back(value);
        denseEntities.push_back(e.id);
        sparse[e.id] = index;
    }

    void remove(EntityHandle e) override {
        assert(has(e));
        uint32_t index = sparse[e.id];
        uint32_t last = (uint32_t)denseData.size() - 1;

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

    EntityHandle entityAt(size_t i) const {
        return EntityHandle{ denseEntities[i], 0 };
    }
};
