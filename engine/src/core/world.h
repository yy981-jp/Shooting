#pragma once;
#include "componentStorage.h"
#include "../components/all.h"
#include "../core/entityManager.h"

struct World {
    std::vector<IStorage*> storages = { // destroyのためにすべてのcomponentのアドレスを格納
        &sprite, &transform
    };

    ComponentStorage<Sprite> sprite;
    ComponentStorage<Transform> transform;

    void destroy(EntityHandle e) {
        entMgr.destroy(e);
        for (const auto& storage: storages) {
            storage->remove(e);
        }
    }
} world;
