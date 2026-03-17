#pragma once
#include "def.h"
#include "entityManager.h"
#include "collider.h"

#include <deque>


template<typename T>
struct EntityBase {
    EntityHandle ent_h;
    ColliderHandle col_h;
    MotionState ms;

    EntityBase(const EntityHandle& e, const ColliderHandle& c, const vec2f& pos)
        : ent_h(e), col_h(c), ms(pos) {}
};

struct IEntityManagerBase {
protected:
    static inline std::vector<IEntityManagerBase*> list; // 各EntityManager

    virtual void update(float dt, GCMS& gcm) = 0;
    virtual void draw(const Renderer* renderer) const = 0;

public:
    virtual ~IEntityManagerBase() {
        auto it = std::find(list.begin(), list.end(), this);
        if (it != list.end()) list.erase(it);
    }

    static void updateAll(float dt, GCMS& gcm) {
        for (const auto& e: list) {
            e->update(dt,gcm);
        }
    }
    static void drawAll(const Renderer* renderer) {
        for (const auto& e: list) {
            e->draw(renderer);
        }
    }
};

template<typename T>
struct EntityManagerBase: IEntityManagerBase {
    std::deque<T> objects;

    EntityManagerBase() {
        IEntityManagerBase::list.push_back(this);
    }

    virtual void update(float dt, GCMS& gcm) override {
        for (size_t i = 0; i < objects.size();) {
            if (!objects[i].update(dt, gcm)) {
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

    void draw(const Renderer* renderer) const override {
        for (const auto& object: objects) {
            object.draw(renderer);
        }
        // renderer->flush();
    }
};
