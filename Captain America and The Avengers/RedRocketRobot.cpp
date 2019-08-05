﻿#include "RedRocketRobot.h"
#include "Shield.h"
#include "FrameWork/SoundManager.h"
void RedRocketRobot::Update(float dt)
{
	Enemy::Update(dt);
	Player *player = Player::GetInstance();
	this->current_animation->Update(dt);
	if (this->IsExplode) {
		this->time_explode += dt;
		this->current_state = RedRocketRobotState::none;
		this->current_animation = explode_ani;
		this->SetVelocityX(0.0f);

		if (this->time_explode >= TIME_EXPLODE) {
			this->IsDead = true;
			this->IsActive = false;
		}
		return;
	}
	if (this->IsBeaten) {
		this->current_state = RedRocketRobotState::beaten;
		this->current_animation = beaten_ani;
		this->SetVelocityX(0.0f);
		this->time_beaten += dt;
		if (this->time_beaten >= TIME_BEATEN) {
			this->IsExplode = true;
			SoundManager::GetInstance()->Play(SoundManager::SoundList::entity_explode);
			
		}
		if (this->GetMoveDirection() == Entity::Entity_Direction::LeftToRight) {
			this->position.x -= 1;

		}
		else {
			this->position.x += 1;
		}
		return;
	}
	
	// noob
	if (this->level == Level::stupid) {
		this->UpdateStupidLevel(dt);
	}
	/*Quy luật của normal lever
	- Đi tới 1 địa điểm rồi đứng yên, sau đó change state ngồi/ đứng mỗi 2s, 
	bắn tên lửa lúc bắt đầu change state.
	*/
	if (this->level == Level::normal) {
		this->UpdateNormalLevel(dt);

	}
	/*Quy luật của clever lever
	- Move direction không thay đổi, move direction sẽ xét 1 lần duy nhất (set clever direction) tại lúc spawn,
	clever direction ngược hướng với player.
	- Clever chỉ cần distance_goto và position_spawn (mới phát hiện), position_goto được cập nhật mỗi khi đáp
	đất sau khi nhảy. Khi đi đủ distance_goto thì sẽ bắt đầu đi lùi. Qúa trình sẽ khởi đầu loop ở position_goto
	sau đó đi lùi cho đến khi đủ distance_goto, đổi chiều velocity.
	- Không giới hạn số lượt nhảy, nhảy mỗi khi không còn va chạm với ground và trạng thái đang running (tức là
	đang chạy nhưng hụt chân) hoặc bị beaten (beaten sẽ đẩy lùi, khi đó có thể ko va chạm với ground) nên nhảy.
	- Chỉ được thôi loop khi bắn tên lửa lúc ngồi
	*/
	if (this->level == Level::clever) {
		this->UpdateCleverLevel(dt);
	}
	if (this->level == Level::master) {
		this->UpdateMasterLevel(dt);
	}
}

void RedRocketRobot::UpdateStupidLevel(float dt)
{

	if (IsChamDatLanDau == false)
	{
		if (this->IsCollisionWithGround(dt)) {
			this->SetVelocityY(0);
			this->position_spawn = this->position;
			IsChamDatLanDau = true;

		}
		else {
			this->SetVelocityY(RED_ROCKET_ROBOT_VELOCITY);

		}
		return;
	}
	this->UpdateUPDOWNNormalState(dt);
	return;
}

void RedRocketRobot::UpdateNormalLevel(float dt)
{

	this->UpdateUPDOWNNormalState(dt);
	if ((Player::GetInstance()->GetPosition().x - this->GetPosition().x > 0)) {
		this->SetMoveDirection(Entity::Entity_Direction::LeftToRight);
	}
	else {
		this->SetMoveDirection(Entity::Entity_Direction::RightToLeft);
	}
	// Khi không còn va chạm với mặt đất, cho phép một lần nhảy
	if (IsChamDatLanDau == false)
	{
		if (this->IsCollisionWithGround(dt)) {
			this->SetVelocityY(0);
			this->position_spawn = this->position;
			IsChamDatLanDau = true;

		}
		else {
			this->SetVelocityY(RED_ROCKET_ROBOT_VELOCITY);

		}
		return;
	}
	// Cập nhật tọa độ khi đang nhảy
	if (this->current_state == RedRocketRobotState::jumping) {
		if (this->IsCollisionWithGround(dt) && this->IsJumpingFirst > 5) {
			this->current_state = RedRocketRobotState::idle;
			this->current_animation = idle_ani;
			return;
		}
		this->IsJumpingFirst++;
		this->SetMoveDirection(this->goto_direction);
		if (this->GetMoveDirection() == Entity::Entity_Direction::LeftToRight) {
			this->SetVelocityX(RED_ROCKET_ROBOT_VELOCITY_X);

		}
		else {
			//this->SetPositionX(this->position.x - RED_ROCKET_ROBOT_VELOCITY_X * dt/2);
			this->SetVelocityX(RED_ROCKET_ROBOT_VELOCITY_X);

		}
		this->SetVelocityY(RED_ROCKET_ROBOT_VELOCITY);
		this->SetPositionY(e->GetYFromX(this->GetPosition().x));
		return;
	}
	// Bắt việc shield có được ném hay không, thì nhảy để né
	//if (Player::GetInstance()->GetCurrentState() == PlayerState::throwing
		
	// 
	if (Shield::GetInstance()->GetShieldState()->GetCurrentState() == ShieldState::NameState::ShieldAttack
		&& this->IsJumping == false 
		&& this->current_state == RedRocketRobotState::running
		&& (this->GetMoveDirection() != Player::GetInstance()->GetMoveDirection())) {
		this->IsJumping = true;
		this->current_state = RedRocketRobotState::jumping;
		this->current_animation = ducking_ani;
		int move = 0;
		if (this->position.x - this->position_goto.x > 0) {
			move = -1;
		}
		else {
			move = 1;

		}
		e = new Equation(
			this->position,
			D3DXVECTOR2(this->position.x + move * 50, this->position.y));

		return;
	}

	// Đi tới position togo, nếu đã nhảy rồi thì không đi nữa, hoặc đã đi tới điểm cần tới

	if (abs(position.x - position_goto.x) >= 10.0f && this->IsJumpingFirst == 0) {
		this->SetVelocityX(RED_ROCKET_ROBOT_VELOCITY_X);
		this->current_state = RedRocketRobotState::running;
		this->current_animation = this->running_ani;
		this->SetMoveDirection(this->goto_direction);
	}
	else {
		
		if ((IsLoop == false && this->current_state == RedRocketRobotState::running) || this->IsJumpingFirst > 0) {
			this->SetVelocityX(0);
			this->SetVelocityY(0);
		
			this->IsLoop = true;
		}
		if (this->current_state == RedRocketRobotState::running) {
			this->current_state = RedRocketRobotState::idle;
			this->current_animation = idle_ani;

		}
	}
}

void RedRocketRobot::UpdateMasterLevel(float dt)
{
	
	// Khi không còn va chạm với mặt đất, cho phép một lần nhảy
	if (IsChamDatLanDau == false)
	{
		if (this->IsCollisionWithGround(dt)) {
			this->SetVelocityY(0);
			this->position_spawn = this->position;
			IsChamDatLanDau = true;

		}
		else {
			this->SetVelocityY(RED_ROCKET_ROBOT_VELOCITY);

		}
		return;
	}
	// Cập nhật tọa độ khi đang nhảy
	if (this->current_state == RedRocketRobotState::jumping) {
		if (this->IsCollisionWithGround(dt) && this->IsJumpingFirst > 5) {
			this->SetVelocityX(RED_ROCKET_ROBOT_VELOCITY_X);
			this->current_state = RedRocketRobotState::running;
			this->current_animation = running_ani;
			return;
		}
		this->IsJumpingFirst++;
		this->SetMoveDirection(this->goto_direction);
		this->SetVelocityY(RED_ROCKET_ROBOT_VELOCITY);
		this->SetPositionY(e->GetYFromX(this->GetPosition().x));
		return;
	}

	if (Shield::GetInstance()->GetShieldState()->GetCurrentState() == ShieldState::NameState::ShieldAttack
		&& this->IsJumping == false
		&& this->current_state == RedRocketRobotState::running
		&& (this->GetMoveDirection() != Player::GetInstance()->GetMoveDirection())) {
		this->IsJumping = true;
		this->current_state = RedRocketRobotState::jumping;
		this->current_animation = ducking_ani;
		int move = 0;
		if (this->position.x - this->position_goto.x > 0) {
			move = -1;
		}
		else {
			move = 1;

		}
		e = new Equation(
			this->position,
			D3DXVECTOR2(this->position.x + move * 50, this->position.y));

		return;
	}

	// Đi tới position togo, nếu đã nhảy rồi thì không đi nữa, hoặc đã đi tới điểm cần tới

	if (abs(position.x - position_goto.x) >= 10.0f && this->IsJumpingFirst == 0) {
		this->SetVelocityX(RED_ROCKET_ROBOT_VELOCITY_X);
		this->current_state = RedRocketRobotState::running;
		this->current_animation = this->running_ani;
		this->SetMoveDirection(this->goto_direction);
	}
	else {

		if ((IsLoop == false && this->current_state == RedRocketRobotState::running) || this->IsJumpingFirst > 0) {
			this->SetVelocityY(0);
			this->IsLoop = true;
		}
		
	}
}

void RedRocketRobot::UpdateCleverLevel(float dt)
{

	if (this->current_state == RedRocketRobotState::walking_throwing) {
		this->UpdateWalkingThrowState(dt);
		return;
	}
	if (this->current_state == RedRocketRobotState::running) {
		this->UpdateRunningState(dt);
		return;
	}
	if (this->current_state == RedRocketRobotState::idle || this->current_state == RedRocketRobotState::ducking) {
		this->UpdateIdleState(dt);
		return;

	}
	if (this->current_state == RedRocketRobotState::jumping) {
		this->UpdateJumpingState(dt);
		return;

	}
}

int RedRocketRobot::OnCollision(Entity* obj, float dt)
{
	if (Camera::GetInstance()->GetCameraFreeze() == true) {
		if (Player::GetInstance()->IsBornRocketRobot == false) {
			if (this->hp <= 0) {
				Player::GetInstance()->number_rocket_robot += 1;
				Player::GetInstance()->IsBornRocketRobot = true;
				this->IsExplode = true;
				SoundManager::GetInstance()->Play(SoundManager::SoundList::entity_explode);
			}
			if (this->GetPosition().x <= 260 && this->GetPosition().y <= 200) {
				Player::GetInstance()->IsBornRocketRobot = true;
				this->IsExplode = true;
				SoundManager::GetInstance()->Play(SoundManager::SoundList::entity_explode);
			}
		}
	}
	return Enemy::OnCollision(obj, dt);
}

void RedRocketRobot::Draw()
{

	Enemy::Draw();
}

BoundingBox RedRocketRobot::GetBoundingBox()
{
	SIZE size;
	size.cx = 24;
	size.cy = 46;
	return BoundingBox(Entity::position, size, Entity::velocity.x, Entity::velocity.y);
}

bool RedRocketRobot::IsCollisionWithGround(float dt, int delta_y)
{
	SIZE FootSize;
	FootSize.cx = RED_ROCKET_ROBOT_SIZE_WIDTH;
	FootSize.cy = RED_ROCKET_ROBOT_FOOT_HEIGHT;
	BoundingBox foot(D3DXVECTOR2(position.x, position.y - delta_y), FootSize, velocity.x*dt, velocity.y*dt);
	auto Checker = Collision::getInstance();
	vector<Entity*> obj = *SceneManager::GetInstance()->GetCurrentScene()->GetCurrentMap()->GetMapObj();

	if (foot.vy == 0)
	{
		for (auto item : obj)
		{
			if (item->GetTag() == Entity::Entity_Tag::ground && Checker->IsCollide(foot, BoundingBox(item->GetPosition(), item->GetSize(), 0, 0)))
				return true;
		}
		return false;
	}

	CollisionOut tmp;
	BoundingBox box2;
	for (auto item : obj)
	{
		if (item->GetTag() == Entity::Entity_Tag::ground)
		{
			box2 = BoundingBox(item->GetPosition(), item->GetSize(), 0, 0);
			tmp = Checker->SweptAABB(foot, box2);
			if (tmp.side == CollisionSide::bottom)
			{
				position.y = item->GetPosition().y + RED_ROCKET_ROBOT_SIZE_HEIGHT / 2;
				return true;
			}
		}
	}
	return false;
}

RedRocketRobot::RedRocketRobot(int level, D3DXVECTOR2 position_spawn, D3DXVECTOR2 position_goto, bool IsCrossed) : Enemy()
{
	this->SetTag(Entity::Entity_Tag::redrobotrocket);
	this->SetType(Entity::Entity_Type::enemy_type);
	this->idle_ani = new Animation(RedRocketRobot::RedRocketRobotState::idle, 2);
	this->running_ani = new Animation(RedRocketRobot::RedRocketRobotState::running, 3);
	this->ducking_ani = new Animation(RedRocketRobot::RedRocketRobotState::ducking, 2);
	this->beaten_ani = new Animation(RedRocketRobot::RedRocketRobotState::beaten, 1);
	//
	this->idle_ani->SetTime(0.5);
	this->running_ani->SetTime(0.1);
	this->ducking_ani->SetTime(0.5);
	//
	this->hp = RED_ROCKET_ROBOT_HP;
	this->IsChamDatLanDau = false;
	this->IsChamLanHai = false;
	this->NumberRocket = 0;
	this->IsLoop = false;
	this->IsJumping = false;
	this->position = position_spawn;
	this->time_ducking = 0.0f;
	this->IsDucking = false;
	this->IsRunning = false;
	this->IsIdle = true;
	this->current_state = RedRocketRobotState::jumping;
	this->previous_state = RedRocketRobotState::idle;
	this->current_animation = idle_ani;
	this->level = Level::stupid;
	this->SetVelocityY(-5.0f);
	this->SetVelocityX(0.0f);
	this->rocket_active = 0;
	this->IsLockChangeRocket;
	this->IsJumpingFirst = 0;
	this->IsLoopClever = false;
	this->IsCrossed = IsCrossed;
	this->IsFire = true;
	this->distance = 0;
	this->Update_position_one_time = false;
	this->IsCapNhatVanToc = true;
	this->IsCapNhatPositionMotLan = false;
	this->IsGray = IsGray;
	//this->time_beaten = 0;
	this->time_explode = 0;
	this->IsExplode = false;
	switch (level) {
	case 0:
		this->level = Level::stupid;
		break;
	case 1:
		this->level = Level::normal;
		break;
	case 2:
		this->level = Level::clever;
		break;
	case 3:
		this->level = Level::master;
	}
	this->position_spawn = position_spawn;
	this->position_goto = position_goto;
	this->distance = 0;
	this->IsLockClever = false;
	this->IsLockDuckingClever = false;
	this->IsLui = -1;
	//
	if (position_spawn.x > position_goto.x) {
		this->SetMoveDirection(Entity::Entity_Direction::RightToLeft);
		this->goto_direction = Entity::Entity_Direction::RightToLeft;

	}
	else {
		this->SetMoveDirection(Entity::Entity_Direction::LeftToRight);
		this->goto_direction = Entity::Entity_Direction::LeftToRight;
	}
	// clever zone
	if (this->level == Level::clever) {
		if (this->position_spawn.x > Player::GetInstance()->GetPosition().x) {
			this->clever_direction = Entity::Entity_Direction::RightToLeft;
			this->position_loop = D3DXVECTOR2(this->position_spawn.x - 4, this->position_spawn.y);
		}
		else {
			this->clever_direction = Entity::Entity_Direction::LeftToRight;
			this->position_loop = D3DXVECTOR2(this->position_spawn.x + 4, this->position_spawn.y);

		}

		this->SetMoveDirection(this->clever_direction);
		this->current_state = RedRocketRobotState::walking_throwing;
		this->current_animation = running_ani;
	}
	// normal zone
	else
	{
		if (this->level == Level::normal || this->level == Level::master) {
			this->current_state = RedRocketRobotState::idle;
			this->current_animation = idle_ani;
			this->previous_state = RedRocketRobotState::idle;
		}
		else {
			this->current_state = RedRocketRobotState::idle;
			this->current_animation = idle_ani;
			this->previous_state = RedRocketRobotState::jumping;
		}
	}
}

void RedRocketRobot::UpdateRunningCleverState(float dt)
{
	if (this->clever_direction == Entity::Entity_Direction::RightToLeft) {
		this->position.x -= RED_ROCKET_ROBOT_VELOCITY_X * dt;
	}
	else {
		this->position.x += RED_ROCKET_ROBOT_VELOCITY_X * dt;

	}
}

void RedRocketRobot::UpdateJumpingCleverState(float dt)
{

	if (IsCapNhatVanToc) {
		e = new Equation(
			this->position,
			D3DXVECTOR2(this->position.x + (this->clever_direction == Entity::Entity_Direction::LeftToRight ? 1 : -1) * 50, this->position.y));
		IsCapNhatVanToc = false;
	}
	if (this->clever_direction == Entity::Entity_Direction::RightToLeft) {
		this->position.x -= RED_ROCKET_ROBOT_VELOCITY_X * dt;
	}
	else {
		this->position.x += RED_ROCKET_ROBOT_VELOCITY_X * dt;

	}
	this->position.y = e->GetYFromX(this->position.x);
	if (this->IsCollisionWithGround(dt)) {
		this->previous_state = RedRocketRobotState::jumping;
		this->current_state = RedRocketRobotState::idle;
		this->current_animation = idle_ani;

	}
}

void RedRocketRobot::UpdateUPDOWNNormalState(float dt)
{
	if ((Player::GetInstance()->GetPosition().x - this->GetPosition().x > 0)) {
		this->SetMoveDirection(Entity::Entity_Direction::LeftToRight);
	}
	else {
		this->SetMoveDirection(Entity::Entity_Direction::RightToLeft);
	}
	if (this->current_state == RedRocketRobotState::running) {
		this->time_idle = 0.0f;
		this->time_ducking = 0.0f;
	}
	// Thực hiện động tác 2s ngồi một lần 
	if (this->IsIdle && this->current_state == RedRocketRobotState::idle) {
		this->time_idle += dt;
		if (this->IsFire && this->time_idle > 0.016f * 5) {
			// rocket zone
			if (this->GetMoveDirection() == Entity::Entity_Direction::LeftToRight) {
				SceneManager::GetInstance()->GetCurrentScene()->GetCurrentGrid()->AddObject2Cell(
					new RedRocket(
						D3DXVECTOR2(this->position.x + 11.5, this->position.y + 17.5),
						this->IsCrossed,
						this->GetMoveDirection()
					)
				);
			}
			else {
				SceneManager::GetInstance()->GetCurrentScene()->GetCurrentGrid()->AddObject2Cell(
					new RedRocket(
						D3DXVECTOR2(this->position.x - 11.5, this->position.y + 17.5),
						this->IsCrossed,
						this->GetMoveDirection()
					)
				);
			}
			this->IsFire = false;
		}
	}
	if (this->IsDucking && this->current_state == RedRocketRobotState::ducking) {
		this->time_ducking += dt;
		// rocket zone
		if (this->IsFire && this->time_ducking > 0.016f * 5) {
			if (this->GetMoveDirection() == Entity::Entity_Direction::LeftToRight) {
				SceneManager::GetInstance()->GetCurrentScene()->GetCurrentGrid()->AddObject2Cell(
					new RedRocket(
						D3DXVECTOR2(this->position.x + 1.5, this->position.y + 1.5),
						this->IsCrossed,
						this->GetMoveDirection()
					)
				);
			}
			else {
				SceneManager::GetInstance()->GetCurrentScene()->GetCurrentGrid()->AddObject2Cell(
					new RedRocket(
						D3DXVECTOR2(this->position.x - 1.5, this->position.y + 1.5),
						this->IsCrossed,
						this->GetMoveDirection()
					)
				);
			}
			this->IsFire = false;

		}
		
	}
	if (this->time_idle >= 2.0f && this->current_state == RedRocketRobotState::idle) {
		this->IsIdle = false;
		this->IsDucking = true;
		this->current_animation = this->ducking_ani;
		this->current_state = RedRocketRobot::RedRocketRobotState::ducking;
		this->time_idle = 0.0f;
		this->IsFire = true;

	}
	if (this->time_ducking >= 2.0f && this->current_state == RedRocketRobotState::ducking) {
		this->IsDucking = false;
		this->IsIdle = true;
		this->current_animation = this->idle_ani;
		this->current_state = RedRocketRobot::RedRocketRobotState::idle;
		this->time_ducking = 0.0f;
		this->IsFire = true;
	}
}

void RedRocketRobot::UpdateRunningState(float dt)
{
	if (this->previous_state == RedRocketRobotState::jumping) {
		goto CHECK;
	}
	if (this->IsCollisionWithGround(dt, 18) == false
		&& previous_state == RedRocketRobotState::idle) 
	{
		this->current_state = RedRocketRobotState::jumping;
		this->current_animation = ducking_ani;
		this->previous_state = RedRocketRobotState::running;
		return;
	}
	else {
		// thông minh hơn
		/*Debug::PrintOut(L"%d\n", this->IsLui);

		if ((Shield::GetInstance()->GetShieldState()->GetCurrentState() == ShieldState::ShieldAttack
			&& IsLui == -1))
		{
			if ((this->GetMoveDirection() == Entity::Entity_Direction::LeftToRight
				&& this->position.x <= Player::GetInstance()->GetPosition().x)
				|| (this->GetMoveDirection() == Entity::Entity_Direction::RightToLeft
					&& this->position.x >= Player::GetInstance()->GetPosition().x))
			{
				this->current_state = RedRocketRobotState::jumping;
				this->current_animation = ducking_ani;
				this->previous_state = RedRocketRobotState::running;
				this->distance = abs(position.x - position_spawn.x);
				if (this->clever_direction == Entity::Entity_Direction::RightToLeft) {
					this->position_loop = D3DXVECTOR2(this->position.x + this->distance, this->position_goto.y);
				}
				else {
					this->position_loop = D3DXVECTOR2(this->position.x - this->distance, this->position_goto.y);

				}
				return;
			}
		}*/
	}
CHECK:
	if (abs(this->position.x - this->position_loop.x) < 2.0f && IsChamLanHai) {
		IsLui = -1;
		this->current_state = RedRocketRobotState::idle;
		this->current_animation = idle_ani;
		this->previous_state = RedRocketRobotState::running;
	}
	if (abs(this->position.x - this->position_goto.x) < 2.0f) {
		IsLui = 1;
		this->current_state = RedRocketRobotState::idle;
		this->current_animation = idle_ani;
		this->previous_state = RedRocketRobotState::running;
	}
	

	if (IsLui == 0) {
		if (this->clever_direction == Entity::Entity_Direction::RightToLeft) {
			this->position.x -= RED_ROCKET_ROBOT_VELOCITY_X * dt;
		}
		else {
			this->position.x += RED_ROCKET_ROBOT_VELOCITY_X * dt;

		}
	}
	if (IsLui == 1) {
		if (this->clever_direction == Entity::Entity_Direction::RightToLeft) {
			this->position.x += RED_ROCKET_ROBOT_VELOCITY_X * dt;
		}
		else {
			this->position.x -= RED_ROCKET_ROBOT_VELOCITY_X * dt;

		}
	}
	if (IsLui == -1) {
		if (this->clever_direction == Entity::Entity_Direction::RightToLeft) {
			this->position.x -= RED_ROCKET_ROBOT_VELOCITY_X * dt;
		}
		else {
			this->position.x += RED_ROCKET_ROBOT_VELOCITY_X * dt;

		}
	}


}

void RedRocketRobot::UpdateJumpingState(float dt)
{
	if (IsChamDatLanDau == false)
	{
		if (this->IsCollisionWithGround(dt, 18)) {
			this->position_spawn = this->position;
			
			IsChamDatLanDau = true;
			this->current_state = RedRocketRobotState::running;
			this->current_animation = running_ani;
		}
		else {
			this->SetVelocityY(RED_ROCKET_ROBOT_VELOCITY);
		}
		return;
	}
	else {
		if (this->IsChamLanHai == false) {
			this->IsChamLanHai = true;
		}
	}	
	this->UpdateJumpingCleverState(dt);
	if (this->Update_position_one_time == false 
		&& this->IsCollisionWithGround(dt, 18)	
		&& this->position.y != this->position_spawn.y) {
		this->Update_position_one_time = true;
		this->distance = abs(position.x - position_spawn.x);
		if (this->clever_direction == Entity::Entity_Direction::RightToLeft) {
			this->position_loop = D3DXVECTOR2(this->position.x + this->distance, this->position_goto.y);
		}
		else {
			this->position_loop = D3DXVECTOR2(this->position.x - this->distance, this->position_goto.y);

		}
		this->IsCapNhatVanToc = true;
		this->current_state = RedRocketRobotState::idle;
		this->current_animation = idle_ani;
	}
	this->IsDucking = false;
	
}

void RedRocketRobot::UpdateIdleState(float dt)
{
	this->SetVelocity(0, 0);

	if (this->current_state == RedRocketRobotState::idle) {
		this->time_idle += dt;
	}
	if (this->current_state == RedRocketRobotState::ducking) {
		this->time_ducking += dt;
		if (this->IsFire && this->time_ducking > 0.016f * 5) {
			// rocket zone
			if (this->GetMoveDirection() == Entity::Entity_Direction::LeftToRight) {
				SceneManager::GetInstance()->GetCurrentScene()->GetCurrentGrid()->AddObject2Cell(
					new RedRocket(
						D3DXVECTOR2(this->position.x + 1.5, this->position.y + 1.5),
						this->IsCrossed,
						this->GetMoveDirection()
					)
				);
			}
			else {
				SceneManager::GetInstance()->GetCurrentScene()->GetCurrentGrid()->AddObject2Cell(
					new RedRocket(
						D3DXVECTOR2(this->position.x - 1.5, this->position.y + 1.5),
						this->IsCrossed,
						this->GetMoveDirection()
					)
				);
			}
			this->IsFire = false;

		}
	}
	if (this->time_idle >= 2.0f && this->current_state == RedRocketRobotState::idle) {
		this->current_animation = this->ducking_ani;
		this->current_state = RedRocketRobot::RedRocketRobotState::ducking;
		this->time_idle = 0.0f;
		this->IsDucking = true;
	
		this->IsFire = true;
	}
	if (this->time_ducking >= 2.0f && this->current_state == RedRocketRobotState::ducking) {
		this->current_animation = this->running_ani;
		this->current_state = RedRocketRobot::RedRocketRobotState::running;
		this->time_ducking = 0.0f;
		this->previous_state = RedRocketRobotState::idle;
		if (IsCapNhatPositionMotLan == false) {
			if (this->IsLui == 1) {
				this->position.x -= 2;
				this->position_loop = this->position;

			}
			if (this->IsLui == -1) {
				this->position.x += 2;

				this->position_goto = this->position;

			}
			this->IsCapNhatPositionMotLan = true;
			this->IsLui = 1;
		}
		
		this->IsDucking = false;
	
		if (this->clever_direction == Entity::Entity_Direction::RightToLeft) {
			if (abs(this->position.x - this->position_goto.x) < 2.0f) {
				this->position.x += 2;

			}
			if (abs(this->position.x - this->position_loop.x) < 2.0f) {
				this->position.x -= 2;
			}
		}
	
		if (this->clever_direction == Entity::Entity_Direction::LeftToRight) {
			if (abs(this->position.x - this->position_goto.x) < 2.0f) {
				this->position.x -= 2;

			}
			if (abs(this->position.x - this->position_loop.x) < 2.0f) {
				this->position.x += 2;
			}
		}
	}
}

void RedRocketRobot::UpdateWalkingThrowState(float dt)
{
	if (this->IsCollisionWithGround(dt) == false
		&& previous_state == RedRocketRobotState::idle) {
		this->current_state = RedRocketRobotState::jumping;
		this->current_animation = ducking_ani;
	}
	if (this->clever_direction == Entity::Entity_Direction::RightToLeft) {
		this->position.x -= RED_ROCKET_ROBOT_VELOCITY_X * dt;
	}
	else {
		this->position.x += RED_ROCKET_ROBOT_VELOCITY_X * dt;

	}
	
}

RedRocketRobot::~RedRocketRobot()
{
	//if (this->rocket != NULL) delete this->rocket;
}
