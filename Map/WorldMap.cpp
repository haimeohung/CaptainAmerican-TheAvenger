#include "WorldMap.h"
#include "Framework/d3d.h"
#include "Camera.h"
#include <fstream>
#include <math.h>

#include "Framework/Debug.h"

using namespace std;

WorldMap::WorldMap(LPCWSTR Path, int IDTitleMap) : Sprite(IDTitleMap)
{
	Init(Path);
}

WorldMap::WorldMap(LPCWSTR Path, LPCWSTR PathTitleMap) : Sprite(PathTitleMap, D3DCOLOR_XRGB(0, 0, 0))
{
	Init(Path);
}

void WorldMap::Draw()
{
	LPD3DXSPRITE Hander = d3d::GetInstance()->GetSpriteHander();
	D3DXMATRIX OldMatrix;
	Hander->GetTransform(&OldMatrix);

	D3DXVECTOR2 pos = Camera::GetInstance()->GetCameraPosition();
	int gStartX = (int)(pos.x / PIXEL_PER_TITLE);
	int gStartY = Height - (int)(pos.y / PIXEL_PER_TITLE) - GRID_HIGHT_BLOCK - 1;

	Debug::PrintOut(L"%d ", gStartY);

	Hander->Begin(D3DXSPRITE_ALPHABLEND);

	int x = (PIXEL_PER_TITLE >> 1) - ((int)pos.x % PIXEL_PER_TITLE);
	for (int i = gStartX; i <= gStartX + GRID_WIDTH_BLOCK; i++)
	{
		int y = -(PIXEL_PER_TITLE >> 1) + ((int)pos.y % PIXEL_PER_TITLE);
		for (int j = gStartY; j <= gStartY + GRID_HIGHT_BLOCK; j++)
		{
			if (j < 0)
			{
				y += PIXEL_PER_TITLE;
				continue;
			}
			SetRect(Map[j][i]);
			Sprite::SetPosition(x, y);
			Hander->SetTransform(&Matrix);
			Hander->Draw(texture, &rect, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));
			y += PIXEL_PER_TITLE;
		}
		x += PIXEL_PER_TITLE;
	}

	Hander->End();

	Hander->SetTransform(&OldMatrix);
}

void WorldMap::SetRect(int ID)
{
	LONG Left = ID * PIXEL_PER_TITLE;
	Sprite::SetRect(0, PIXEL_PER_TITLE, Left, Left + PIXEL_PER_TITLE);
}

WorldMap::~WorldMap()
{
	for (int i = 0; i < Width; i++)
		delete Map[i];
	delete Map;
}

void WorldMap::Init(LPCWSTR Path)
{
	fstream data(Path, ios_base::in);
	data >> TotalTitle >> Width >> Height;
	Map = new int*[Height];
	for (int i = 0; i < Height; i++)
	{
		Map[i] = new int[Width];
		for (int j = 0; j < Width; j++)
			data >> Map[i][j];
	}
	Camera::GetInstance()->Init(Width * PIXEL_PER_TITLE, Height * PIXEL_PER_TITLE);
}
