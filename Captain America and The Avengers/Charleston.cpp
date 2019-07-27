#include <d3dx9.h>
#include <fstream>
#include "Charleston.h"
#include "Framework/DirectInput.h"
#include "Framework/Debug.h"
#include "Camera.h"
#include "WorldMap.h"
#include "Player.h"
#include "Shield.h"
#include "Framework/DirectInput.h"
#include "SceneManager.h"
#include "CharlestonBoss.h"
#include "RedRocketRobot.h"
#include "ItemsHolder.h"

void Charleston::Update(float dt)
{
	
	Player *player = Player::GetInstance();
	// Update zone
	map->Update(dt);

	grid->Update(dt);

	player->HandleInput(dt);
	player->Update(dt);
	Shield::GetInstance()->Update(dt);
	cam->SetCameraPosition(player->GetPosition());

	auto tmp = DirectInput::GetInstance();
	if (tmp->KeyDown(DIK_N))
	{
		CharlestonBoss *boss = new CharlestonBoss();
		SceneManager::GetInstance()->ReplaceScene(boss);
	}
}

void Charleston::Draw()
{
	map->Draw();
	grid->DrawActivatedObject();
	//Player::GetInstance()->Draw();
}

WorldMap * Charleston::GetCurrentMap()
{
	return this->map;
}

Grid *Charleston::GetCurrentGrid()
{
	return grid;
}

void Charleston::Init()
{
	Player* player = Player::GetInstance();
	player->Init();
	player->SetPosition(50.0f, 100.0f);

	int n, m;
	int tag, posX, posY, tmp;
	fstream data(L"Resources/Map/charleston_map_items_enemy.txt", ios_base::in);
	int *obj;
	Entity *item;
	ItemsHolder *holder;
	data >> n;
	for (int i = 0; i < n; i++)
	{
		data >> tag >> posX >> posY;
		switch (tag)
		{
		case Entity::Entity_Tag::item_container:
			holder = new ItemsHolder(posX, posY);
			data >> m;
			for (int i = 0; i < m; i++)
			{
				data >> tmp;
				holder->PutOnItem(tmp);
			}
			grid->AddObject2Cell(holder);
			continue;
		case Entity::Entity_Tag::redrobotrocket:
			obj = new int[7];
			obj[0] = tag;
			obj[1] = posX;
			obj[2] = posY;
			data >> obj[3] >> obj[4] >> obj[5] >> obj[6];
			grid->AddObject2Cell(posX, posY, obj);
			continue;
		case Entity::Entity_Tag::blue_soldier:
			obj = new int[5];
			obj[0] = tag;
			obj[1] = posX;
			obj[2] = posY;
			data >> obj[3] >> obj[4];
			grid->AddObject2Cell(posX, posY, obj);
			continue;
		default:
			continue;
		}
	}

}

Charleston::Charleston()
{
	map = new CharlestonMap();
	cam = Camera::GetInstance();
	grid = new Grid(map->GetMapSize());
	Charleston::Init();
}

Charleston::~Charleston()
{
	delete map;
	delete grid;
}
