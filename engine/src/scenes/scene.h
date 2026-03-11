#pragma once

#include "../gcms/gcms.h"
#include "../core/input.h"
#include "../graphics/gfx.h"
#include "../graphics/text.h"
#include "../audio/sfx.h"


class Game;

struct SceneContext {
	GCMS* gcms;
	uint8_t* input; // SHTKeyCode
	Renderer* gfx;
	Text* txtgfx;
	SFXManager* sfx;
};

struct IScene {
	virtual void update(SceneContext& ctx, const float dt) = 0;
	virtual void draw(const SceneContext& ctx) const = 0;
	virtual void handleCommand(const GameCommand& cmd, Game& game) = 0;
	virtual ~IScene() = default;
};
