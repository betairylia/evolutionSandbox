#pragma once

#include <SDL.h>
#include <vector>

#include "Agent.h"
#include "GraphNet.h"
#include "PerlinNoise.h"

class SandboxBackend
{
public:
	SandboxBackend(int seed);
	~SandboxBackend();

	bool Init();
	void Update();
	void Render();

	void AddAgent(Agent& agent);
	bool UseItem(Agent& agent, ObjectType item);
	void UpdateMap();

	int getSpawnPointX(int index);
	int getSpawnPointY(int index);

	std::vector<Agent*> m_agentList;

	MapGrid m_map[MAPSIZE_X][MAPSIZE_Y];
	
	float speciesMateLifespanMin[SPECIES_COUNT];
	float speciesMateLifespanGap[SPECIES_COUNT];

	int maxLifeSpan = 0;

	unsigned int time;
	int m_seed;
};

