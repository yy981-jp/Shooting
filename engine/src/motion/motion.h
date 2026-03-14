#pragma once
#include <variant>
#include <vector>

#include "../core/def.h"

#include "LineController.h"
#include "BezierController.h"
#include "WaveDecorator.h"


using MVController = std::variant<
    LineController,
    BezierController
>;

using MVDecorator = std::variant<
    WaveDecorator
>;


/// @brief 1phase
class MotionPipeline {
    MVController base;
    std::vector<MVDecorator> modifiers;
    bool running = true;

public:
    MotionPipeline(const MVController& base): base(base) {}

    void addMover(const MVDecorator& modi) {
        modifiers.push_back(modi);
    }

    /// @return vel
	vec2f update(float dt, MotionState& ms) {
        bool alive = true;
		// Base
		std::visit([&](auto& b){
            if constexpr (requires { b.isRunning(); }) 
                alive &= b.isRunning();
			b.update(dt, ms);
		}, base);

		// Modifier
		for (auto& m : modifiers) {
			std::visit([&](auto& mod){
            if constexpr (requires { mod.isRunning(); }) 
                alive &= mod.isRunning();
			mod.update(dt, ms);
			}, m);
		}

        running = alive;
        vec2f dir = cachesv.getDir(ms.angle + CacheSV::OFFSET);
        vec2f vel = dir * ms.speed;
        return vel;
	};

    bool isRunning() {
        return running;
    }
};


/// @brief phaseの集合体
class MotionSequence {
	std::vector<MotionPipeline> phases;
	size_t index = 0;

public:
	vec2f update(float dt, MotionState& ms) {
		if (index >= phases.size())
			return {0,0};

		auto& cur = phases[index];
		vec2f vel = cur.update(dt, ms);

		if (!cur.isRunning()) {
			index++;
		}

		return vel;
	}

    void addPipeline(const MotionPipeline& mp) {
        phases.push_back(mp);
    }
};
