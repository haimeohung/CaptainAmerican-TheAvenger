#pragma once
#include "PlayerState.h"
#include "Player.h"
class PlayerKickingState : public PlayerState
{
public:
	PlayerKickingState();
	~PlayerKickingState();
	virtual void Update(float dt);
	virtual void Draw();
	virtual void Render();
	virtual void HandleInput(float dt);
};

