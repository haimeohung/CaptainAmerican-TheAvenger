﻿#include "PlayerDivingState.h"
#include "PlayerFlowingState.h"
#include "Framework//Debug.h"
PlayerDivingState::PlayerDivingState()
{
	Player* player = Player::GetInstance();
	player->SetCurrentState(PlayerState::NameState::diving);

	this->current_state = PlayerState::NameState::diving;
	player->SetTimeBuffer(0);
	player->SetVelocity(0, 0);
	player->SetPositionY(44.0f);

	auto tmp = player->GetCurrentAnimation();
	
}
PlayerDivingState::~PlayerDivingState()
{

}

void PlayerDivingState::Update(float dt)
{
	Player* player = Player::GetInstance();
	player->GetCurrentAnimation()->Update(dt);
	player->SetPositionX(player->GetPosition().x - VELOCITY_X / 2 * dt);
}

void PlayerDivingState::Draw()
{

}

void PlayerDivingState::Render()
{
}

void PlayerDivingState::HandleInput(float dt)
{
	Player* player = Player::GetInstance();
	auto keyboard = DirectInput::GetInstance();

	if (keyboard->KeyDown(JUMP_KEY)) {
		player->ChangeState(new PlayerJumpingState());
		return;
	}
	if (keyboard->KeyPress(RIGHT_KEY) && keyboard->KeyPress(DOWN_KEY)) {
		player->ChangeState(new PlayerFlowingState());
		player->SetMoveDirection(Entity::Entity_Direction::LeftToRight);
		player->GetCurrentAnimation()->SetAnimationTime(0.0f);
		player->GetCurrentAnimation()->ResetAnimation();
		return;
	}
	if (keyboard->KeyPress(LEFT_KEY) && keyboard->KeyPress(DOWN_KEY)) {
		player->ChangeState(new PlayerFlowingState());
		player->SetMoveDirection(Entity::Entity_Direction::RightToLeft);
		player->GetCurrentAnimation()->SetAnimationTime(0.0f);
		player->GetCurrentAnimation()->ResetAnimation();
		return;
	}

	if (keyboard->KeyPress(DOWN_KEY)) {
		return;
	}
	
	
	player->ChangeState(new PlayerFlowingState());
	player->GetCurrentAnimation()->SetAnimationTime(0.0f);
	player->GetCurrentAnimation()->ResetAnimation();
	return;
}
