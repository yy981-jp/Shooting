#pragma once
#include "componentStorage.h"
#include "../components/all.h"
#include "../core/entityManager.h"
#include "../graphics/gfx.h"

struct World {
    const Renderer& renderer;

    World(const Renderer& renderer): renderer(renderer) {
        st_player = 
    }

    std::vector<IStorage*> storages = { // destroyのためにすべてのcomponentのアドレスを格納
        &st_sprite, &st_transform
    };

    ComponentStorage<Sprite> st_sprite;
    ComponentStorage<Transform> st_transform;
    ComponentStorage<Player> st_player;

    void destroy(EntityHandle e) {
        entMgr.destroy(e);
        for (const auto& storage: storages) {
            storage->remove(e);
        }
    }


    void system_sprite() { // transformはspriteを前提とする
        for (int i = 0; i < st_sprite.size(); ++i) {
            const auto& spriteID = st_sprite.denseData[i].spriteID;
            const auto& rect = st_transform.denseData[i];
            renderer.drawSprite(spriteID,rect.pos);
        }
    }

    void system_sp_player() {
        
    }
};
