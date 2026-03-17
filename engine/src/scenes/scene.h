#pragma once

#include "../gcms/gcms.h"
#include "../core/input.h"
#include "../graphics/gfx.h"
#include "../audio/sfx.h"


class Game;

struct SceneContext {
	GCMS* gcms;
	uint8_t* key; // KCode
	Renderer* gfx;
	SFXManager* sfx;
};

struct IScene {
	SceneID type;
	virtual void update(SceneContext& ctx, const float dt) = 0;
	virtual void draw(const SceneContext& ctx) const = 0;
	virtual ~IScene() = default;
	IScene(SceneID type): type(type) {}
};
