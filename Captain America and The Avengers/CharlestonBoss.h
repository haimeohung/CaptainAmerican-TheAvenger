#pragma once
#include "Scene.h"
#include "CharlestonBossMap.h"
#include "Camera.h"
#include "Framework/Animation.h"
#include <vector>

class CharlestonBoss : public Scene
{
public:
	void Update(float dt) override;
	void Draw() override;
	WorldMap *GetCurrentMap() override;
	CharlestonBoss();
	~CharlestonBoss();
protected:
	CharlestonBossMap *map;
	Camera *cam;
	
};