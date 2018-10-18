#include "utils.h"

int directionVector[4][2] = { {0, 1}, {0, -1}, {-1, 0}, {1, 0} };
float terrainStaminaCost[6] = { 1.5, 1.0, 0.7, 0.7, 0.7, 10.0 };

int directionMapping[4][4] = { {0, 1, 2, 3}, {1, 0, 3, 2}, {2, 3, 1, 0}, {3, 2, 0, 1}, };

// x, y, w, h
SDL_Rect spriteClips_Terrain[6] =
{
	{ 0,  0, 12, 12},
	{12,  0, 12, 12},
	{24,  0, 12, 12},
	{36,  0, 12, 12},
	{48,  0, 12, 12},
	{60,  0, 12, 12}
};

SDL_Rect spriteClips_Objects[4] =
{
	{ 0, 24, 12, 12},
	{12, 24, 12, 12},
	{24, 24, 12, 12},
	{36, 24, 12, 12}
};

SDL_Rect spriteClips_Species[5] =
{
	{ 0, 12, 12, 12},
	{12, 12, 12, 12},
	{24, 12, 12, 12},
	{36, 12, 12, 12},
	{48, 12, 12, 12}
};
