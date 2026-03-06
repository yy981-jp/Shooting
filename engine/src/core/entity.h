#pragma once
#include "def.h"


template<typename T>
struct EntityBase {
    EntityHandle ent_h;
    ColliderHandle col_h;
    MotionState ms;

    EntityBase(const EntityHandle& e, const ColliderHandle& c, const vec2f& pos)
        : ent_h(e), col_h(c), ms(pos) {}
};

template<typename T>
struct EntityManagerBase {
    std::vector<T> objects;

    template<typename T_E>
    bool callUpdate(T_E& obj, float dt) {
        return obj.update(dt);
    }

    template<typename T_E>
    bool callUpdate(T_E& obj, float dt, GCMS& gcm) {
        if constexpr (requires { obj.update(dt, gcm); })
            return obj.update(dt, gcm);
        else
            return obj.update(dt);
    }
    
    void update(float dt, GCMS& gcm) {
        for (size_t i = 0; i < objects.size();) {
            if (callUpdate(objects[i], dt, gcm)) {
                physWorld.destroy(objects[i].col_h);
                entMgr.destroy(objects[i].ent_h);
                objects[i] = std::move(objects.back());
                entMgr.setPtr(objects[i].ent_h, &objects[i]);
                objects.pop_back();
            } else {
                physWorld.setPos(objects[i].col_h, objects[i].ms.pos);
                ++i;
            }
        }
    }

    void update(float dt) {
        for (size_t i = 0; i < objects.size();) {
            if (callUpdate(objects[i], dt)) {
                physWorld.destroy(objects[i].col_h);
                entMgr.destroy(objects[i].ent_h);
                objects[i] = std::move(objects.back());
                entMgr.setPtr(objects[i].ent_h, &objects[i]);
                objects.pop_back();
            } else {
                physWorld.setPos(objects[i].col_h, objects[i].ms.pos);
                ++i;
            }
        }
    }

    void draw(const Renderer* renderer) const {
        for (const auto& object: objects) {
            object.draw(renderer);
        }
        renderer->flush();
    }
};
