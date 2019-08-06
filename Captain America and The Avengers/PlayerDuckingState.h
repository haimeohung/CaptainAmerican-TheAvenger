#pragma once
#include "PlayerState.h"
#include "Player.h"
#include "PlayerRunningState.h"
#include "PlayerJumpingState.h"
#include "PlayerDuckingPunchingState.h"
#include "PlayerIdleState.h"
#include "Framework//DirectInput.h"
class PlayerDuckingState : public PlayerState
{
public:
	bool IsPunching;
	PlayerDuckingState();
	~PlayerDuckingState();
	virtual void Update(float dt);
	virtual void Draw();
	BoundingBox GetBoundingBox() override;
	virtual void HandleInput(float dt);
};

