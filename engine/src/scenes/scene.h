#pragma once

#include "../core/commands.h"
#include "../core/input.h"
#include "../graphics/gfx.h"
#include "../audio/sfx.h"


class Game;

struct SceneContext {
	GCMS* gcms;
	uint8_t* input; // SHTKeyCode
	Renderer* gfx;
	SFXManager* sfx;
};

struct IScene {
	virtual void update(SceneContext& ctx, const float dt) = 0;
	virtual void draw(const SceneContext& ctx) const = 0;
	virtual void handleCommand(const GameCommand& cmd, Game& game) = 0;
	virtual ~IScene() = default;
};
