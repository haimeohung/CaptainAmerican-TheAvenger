#pragma once
#include "PlayerState.h"
#include "Player.h"
#include "PlayerRunningState.h"
#include "PlayerJumpingState.h"
#include "PlayerShieldUpState.h"
#include "PlayerDuckingState.h"
#include "PlayerThrowingState.h"
#include "PlayerDashingState.h"
#include "Framework//DirectInput.h"
class PlayerIdleState: public PlayerState
{
public:
	PlayerIdleState();
	~PlayerIdleState();
	virtual void Update(float dt);
	virtual void Draw();
	BoundingBox GetBoundingBox() override;
	virtual void HandleInput(float dt);
};

