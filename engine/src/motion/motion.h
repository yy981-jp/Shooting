#pragma once
#include <variant>
#include <vector>

#include "../core/def.h"

#include "BezierController.h"
#include "WaveDecorator.h"


using MVController = std::variant<
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
		// Base
		std::visit([&](auto& b){
            if constexpr (requires { b.isRunning(); }) {
                if (running) {
                    running = false;
                    if (b.isRunning()) running = true;
                }
            }
			b.update(dt, ms);
		}, base);

		// Modifier
		for (auto& m : modifiers) {
			std::visit([&](auto& mod){
                if constexpr (requires { mod.isRunning(); }) {
                    if (running) {
                        running = false;
                        if (mod.isRunning()) running = true;
                    }
                }
				mod.update(dt, ms);
			}, m);
		}

        vec2f dir = cachesv.getDir(ms.angle);
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
