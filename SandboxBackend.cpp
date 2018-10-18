#include "SandboxBackend.h"

SandboxBackend::SandboxBackend(int seed) : m_seed(seed)
{
}


SandboxBackend::~SandboxBackend()
{
}



bool SandboxBackend::Init()
{
	for (int i = 0; i < SPECIES_COUNT; ++i)
	{
		speciesMateLifespanMin[i] = 100.0;
		speciesMateLifespanGap[i] = 50.0;
	}

	// Init map
	PerlinNoise pn(m_seed);

	for (int x = 0; x < MAPSIZE_X; x++)
	{
		for (int y = 0; y < MAPSIZE_Y; y++)
		{
			double noiseValue = pn.noise((double)x / (double)MAPSIZE_X, (double)y / (double)MAPSIZE_Y, 0.5);

			if (noiseValue < 0.20)
			{
				m_map[x][y].terrain = TerrainType::Meadow;
			}
			else if (noiseValue < 0.50)
			{
				m_map[x][y].terrain = TerrainType::Grass;
			}
			else if (noiseValue < 0.70)
			{
				m_map[x][y].terrain = TerrainType::Dirt;
			}
			else if (noiseValue < 0.85)
			{
				m_map[x][y].terrain = TerrainType::Rock;
			}
			else
			{
				m_map[x][y].terrain = TerrainType::Obstacle;
			}

			m_map[x][y].terrainMeta = -1.0;
			m_map[x][y].object = ObjectType::None;
		}
	}

	// Update the map initially for 20 steps
	for (int i = 0; i < 20; i++)
	{
		for (int x = 0; x < MAPSIZE_X; x++)
		{
			for (int y = 0; y < MAPSIZE_Y; y++)
			{
				// Clear temporal agent data in map
				m_map[x][y].hasAgentTalking = false;
				m_map[x][y].agentSpecies = 0;
				m_map[x][y].currentGridAgent = nullptr;

				// Spawn objects
				if (m_map[x][y].object == ObjectType::None)
				{
					switch (m_map[x][y].terrain)
					{
						// Forest area, hard to move, but produce lots of foods.
					case TerrainType::Meadow:
						int RNG = rand() % 1000;
						if (RNG < 5)
						{
							m_map[x][y].object = ObjectType::Nut;
						}
						else if (RNG < 15)
						{
							m_map[x][y].object = ObjectType::Food;
						}
						else if (RNG < 30)
						{
							m_map[x][y].object = ObjectType::Seed;
						}
						break;

					case TerrainType::Grass:
						int RNG = rand() % 1000;
						if (RNG < 8)
						{
							m_map[x][y].object = ObjectType::Food;
						}
						else if (RNG < 15)
						{
							m_map[x][y].object = ObjectType::Seed;
						}
						break;

					case TerrainType::Rock:
						int RNG = rand() % 1000;
						if (RNG < 20)
						{
							m_map[x][y].object = ObjectType::Stone;
						}
						break;
					}
				}
			}
		}
	}

	// Randomly place agents
	for (int i = 0; i < MINIMAL_AGENT_COUNT; i++)
	{
		Agent* agent = new Agent((*new Gene()), rand() % SPECIES_COUNT);

		agent->x = rand() % MAPSIZE_X;
		agent->y = rand() % MAPSIZE_Y;

		while (m_map[agent->x][agent->y].terrain == TerrainType::Obstacle || m_map[agent->x][agent->y].currentGridAgent != nullptr)
		{
			agent->x = rand() % MAPSIZE_X;
			agent->y = rand() % MAPSIZE_Y;
		}

		AddAgent(*agent);
	}

	return true;
}

void SandboxBackend::Update()
{
	#pragma region UpdateAgents
	// Update all agent
	for (std::vector<Agent&>::iterator pAgent = m_agentList.begin(); pAgent != m_agentList.end(); ++pAgent)
	{
		InputSignal inputSignal;

		// VISION (and some TOUCH)
		int VWD2 = VISION_WIDTH / 2;
		int VDD2 = VISION_DEPTH / 2;
		for (int dstX = 0; dstX < VISION_WIDTH; dstX++)
		{
			for (int dstY = 0; dstY < VISION_DEPTH; dstY++)
			{
				int srcX = 0, srcY = 0;

				switch (pAgent->faceDirection)
				{

				// Face to Y+
				case OutputDirection::Up:
					srcX = dstX - VWD2 + pAgent->x;
					srcY = dstY - 1 + pAgent->y;
					break;
				
				// Face to Y-
				case OutputDirection::Down:
					srcX = -dstX + VWD2 + pAgent->x;
					srcY = -dstY + 1 + pAgent->y;
					break;

				// Face to X+
				case OutputDirection::Right:
					srcX = dstY - 1 + pAgent->x;
					srcY = -dstX + VWD2 + pAgent->y;
					break;

				// Face to X-
				case OutputDirection::Left:
					srcX = -dstY + 1 + pAgent->x;
					srcY = dstX - VWD2 + pAgent->y;
					break;
				}

				// Boundary check
				if (srcX < 0 || srcX > MAPSIZE_X || srcY < 0 || srcY > MAPSIZE_Y)
				{
					// Terrain channel
					inputSignal.vision[dstX * (VISION_DEPTH * VISION_CHANNEL) + dstY * (VISION_CHANNEL)+0] = TerrainType::Obstacle;

					// Object and agent talking channel
					inputSignal.vision[dstX * (VISION_DEPTH * VISION_CHANNEL) + dstY * (VISION_CHANNEL)+1] = ObjectType::None;

					// Agent channel
					inputSignal.vision[dstX * (VISION_DEPTH * VISION_CHANNEL) + dstY * (VISION_CHANNEL)+2] = 0;
				}

				// Terrain channel
				inputSignal.vision[dstX * (VISION_DEPTH * VISION_CHANNEL) + dstY * (VISION_CHANNEL)+0] = 
					(float)(m_map[srcX][srcY].terrain) / (float)(TerrainType::Obstacle);
				
				// Object and agent talking channel
				inputSignal.vision[dstX * (VISION_DEPTH * VISION_CHANNEL) + dstY * (VISION_CHANNEL)+1] = 
					(float)(m_map[srcX][srcY].hasAgentTalking ? ObjectType::AgentSaying : m_map[srcX][srcY].object) / (float)(ObjectType::AgentSaying);
				
				// Agent channel
				inputSignal.vision[dstX * (VISION_DEPTH * VISION_CHANNEL) + dstY * (VISION_CHANNEL)+2] = 
					(float)(m_map[srcX][srcY].agentSpecies) / (float)SPECIES_COUNT;

				// Its own grid
				if (dstX == VWD2 && dstY == 1)
				{
					inputSignal.touch[0] = m_map[srcX][srcY].object == ObjectType::None ? 0 : 1;
					inputSignal.touch[1] = (float)(m_map[srcX][srcY].object) / (float)(ObjectType::AgentSaying);
					inputSignal.touch[4] = (float)(m_map[srcX][srcY].terrain) / (float)(TerrainType::Obstacle);
				}
			}
		}

		// TOUCH
		inputSignal.touch[2] = pAgent->handObject == ObjectType::None ? 0 : 1;
		inputSignal.touch[3] = (float)(pAgent->handObject) / (float)(ObjectType::AgentSaying);

		// STATUS
		// Health %
		inputSignal.status[0] = pAgent->currentHealth / pAgent->maxHealth;

		// Current Health
		inputSignal.status[1] = 0;

		// Health cost
		inputSignal.status[2] = (pAgent->prevHealth - pAgent->currentHealth) / pAgent->maxHealth;

		// Stamina %
		inputSignal.status[3] = pAgent->currentStamina / pAgent->maxStamina;

		// Current Stamina
		inputSignal.status[4] = 0;

		// Stamina cost
		inputSignal.status[5] = (pAgent->prevStamina - pAgent->currentStamina) / pAgent->maxStamina;

		// Mateable
		if ((int)((pAgent->lifetime - speciesMateLifespanMin[pAgent->species]) / speciesMateLifespanGap[pAgent->species]) > pAgent->mateCount)
		{
			inputSignal.status[6] = 1;
		}
		else
		{
			inputSignal.status[6] = 0;
		}

		pAgent->AgentTick(inputSignal);
		pAgent->_tickCount++;

		// Record previous state right after it has been consumed
		pAgent->prevHealth = pAgent->currentHealth;
		pAgent->prevStamina = pAgent->currentStamina;
	}
	#pragma endregion

	#pragma region DoAction
	// Collect agent actions, do them and update agent
	for (std::vector<Agent&>::iterator pAgent = m_agentList.begin(); pAgent != m_agentList.end(); ++pAgent)
	{
		bool agentHasAction = false;
		AgentOutput agentAction;

		// Time for it to act!
		if (pAgent->_tickCount >= pAgent->actionTickCount)
		{
			agentHasAction = true;
			agentAction = pAgent->DoAction();

			// update agent status
			pAgent->faceDirection = (OutputDirection)directionMapping[pAgent->faceDirection][agentAction.direction];

			// Shut up
			pAgent->talking = false;

			switch (agentAction.action)
			{

			// Walk with direction
			case OutputAction::Move:
				if (pAgent->useStamina(1 * terrainStaminaCost[m_map[pAgent->x][pAgent->y].terrain]))
				{
					int newX = pAgent->x + directionVector[pAgent->faceDirection][0], 
						newY = pAgent->y + directionVector[pAgent->faceDirection][1];

					// Boundary check
					if (newX < 0 || newX > MAPSIZE_X || newY < 0 || newY > MAPSIZE_Y)
					{
						break;
					}

					// Obstacle check
					if (m_map[newX][newY].terrain == TerrainType::Obstacle || m_map[newX][newY].agentSpecies != 0)
					{
						break;
					}

					pAgent->x = newX;
					pAgent->y = newY;
				}
				break;

			// Run with direction
			case OutputAction::Run:
				if (pAgent->useStamina(3 * terrainStaminaCost[m_map[pAgent->x][pAgent->y].terrain]))
				{
					int newX = pAgent->x + 2 * directionVector[pAgent->faceDirection][0],
						newY = pAgent->y + 2 * directionVector[pAgent->faceDirection][1];

					// Boundary check
					if (newX < 0 || newX > MAPSIZE_X || newY < 0 || newY > MAPSIZE_Y)
					{
						break;
					}

					// Obstacle check
					if (m_map[newX][newY].terrain == TerrainType::Obstacle || m_map[newX][newY].agentSpecies != 0)
					{
						break;
					}

					pAgent->x = newX;
					pAgent->y = newY;
				}
				break;

			// Stay
			case OutputAction::Stay:
				// Restore health if stamina > 60%
				if (pAgent->currentStamina >= pAgent->maxStamina * 0.6f)
				{
					pAgent->currentHealth += 1;
					if (pAgent->currentHealth > pAgent->maxHealth)
					{
						pAgent->currentHealth = pAgent->maxHealth;
					}
				}
				break;

			// Talking
			case OutputAction::Say:
				pAgent->talking = true;
				break;

			// Pick & Drop
			case OutputAction::Pick_Drop:

				// If it holds an item in its little cute hand
				if (pAgent->handObject != ObjectType::None)
				{
					// And the ground is clear
					if (m_map[pAgent->x][pAgent->y].object == ObjectType::None)
					{
						// Drop it on the ground
						m_map[pAgent->x][pAgent->y].object = pAgent->handObject;
						pAgent->handObject = ObjectType::None;
					}
				}
				else
				{
					// Pick foot item
					if (m_map[pAgent->x][pAgent->y].object != ObjectType::None)
					{
						pAgent->handObject = m_map[pAgent->x][pAgent->y].object;
						m_map[pAgent->x][pAgent->y].object = ObjectType::None;
					}
				}

				break;

			// Attack & Use
			case OutputAction::Attack_Use:

				// If it holds an item in its little cute hand
				if (pAgent->handObject != ObjectType::None)
				{
					if (UseItem(*pAgent, pAgent->handObject))
					{
						pAgent->handObject = ObjectType::None;
					}
				}
				// Or there are some object on the ground
				else if (m_map[pAgent->x][pAgent->y].object != ObjectType::None)
				{
					if(UseItem(*pAgent, m_map[pAgent->x][pAgent->y].object))
					{
						m_map[pAgent->x][pAgent->y].object = ObjectType::None;
					}
				}
				// So we attack finally and let the world peace again.
				else
				{
					int newX = pAgent->x + directionVector[pAgent->faceDirection][0],
						newY = pAgent->y + directionVector[pAgent->faceDirection][1];
					
					// Boundary check
					if (newX < 0 || newX > MAPSIZE_X || newY < 0 || newY > MAPSIZE_Y)
					{
						break;
					}

					Agent* attackTarget = m_map[newX][newY].currentGridAgent;

					if (pAgent->useStamina(3))
					{
						if (attackTarget != nullptr)
						{
							// we damaged our target!
							attackTarget->currentHealth -= 1; // TODO: -= pAgent->strength
						}
					}
				}

				break;
			}
		}
	}
	#pragma endregion

	#pragma region UpdateMap
	// Update the map
	for (int x = 0; x < MAPSIZE_X; x++)
	{
		for (int y = 0; y < MAPSIZE_Y; y++)
		{
			// Clear temporal agent data in map
			m_map[x][y].hasAgentTalking = false;
			m_map[x][y].agentSpecies = 0;
			m_map[x][y].currentGridAgent = nullptr;

			// Spawn objects
			if (m_map[x][y].object == ObjectType::None)
			{
				switch (m_map[x][y].terrain)
				{
					// Forest area, hard to move, but produce lots of foods.
				case TerrainType::Meadow:
					int RNG = rand() % 1000;
					if (RNG < 5)
					{
						m_map[x][y].object = ObjectType::Nut;
					}
					else if (RNG < 15)
					{
						m_map[x][y].object = ObjectType::Food;
					}
					else if (RNG < 30)
					{
						m_map[x][y].object = ObjectType::Seed;
					}
					break;

				case TerrainType::Grass:
					int RNG = rand() % 1000;
					if (RNG < 8)
					{
						m_map[x][y].object = ObjectType::Food;
					}
					else if (RNG < 15)
					{
						m_map[x][y].object = ObjectType::Seed;
					}
					break;

				case TerrainType::Rock:
					int RNG = rand() % 1000;
					if (RNG < 20)
					{
						m_map[x][y].object = ObjectType::Stone;
					}
					break;
				}
			}

			// Update seeds
			if (m_map[x][y].terrain == TerrainType::SeedDirt && m_map[x][y].terrainMeta > 0)
			{
				m_map[x][y].terrainMeta -= 1;
				if (m_map[x][y].terrainMeta <= 0)
				{
					m_map[x][y].terrain = TerrainType::Dirt;

					// Spawn foods
					int foodCount = 2 + rand() % 3;
					int seedCount = 0 + rand() % 3;
					int cnt = 0;
					for (int i = 0; i < foodCount; i++)
					{
						while (cnt < 25)
						{
							int dstX = x + getSpawnPointX(cnt);
							int dstY = y + getSpawnPointY(cnt);

							cnt++;

							if (m_map[dstX][dstY].terrain != TerrainType::Obstacle && m_map[dstX][dstY].object == ObjectType::None)
							{
								m_map[dstX][dstY].object = ObjectType::Food;
								break;
							}
						}
					}
					for (int i = 0; i < seedCount; i++)
					{
						while (cnt < 25)
						{
							int dstX = x + getSpawnPointX(cnt);
							int dstY = y + getSpawnPointY(cnt);

							cnt++;

							if (m_map[dstX][dstY].terrain != TerrainType::Obstacle && m_map[dstX][dstY].object == ObjectType::None)
							{
								m_map[dstX][dstY].object = ObjectType::Seed;
								break;
							}
						}
					}
				}
			}
		}
	}
	#pragma endregion

	#pragma region UpdateAgentStatus
	// Update status for agents and place their pointers in map
	for (std::vector<Agent&>::iterator pAgent = m_agentList.begin(); pAgent != m_agentList.end();)
	{
		// You HUNGER!
		if (!pAgent->useStamina(1))
		{
			// People die if they don't eat
			pAgent->currentHealth -= 0.1;
		}

		// Breeding
		// TODO: breed by 2 parents
		if ((int)((pAgent->lifetime - speciesMateLifespanMin[pAgent->species]) / speciesMateLifespanGap[pAgent->species]) > pAgent->mateCount)
		{
			// Place to nearest available place
			int cnt = 0;

			while (cnt < 25)
			{
				int dstX = pAgent->x + getSpawnPointX(cnt);
				int dstY = pAgent->y + getSpawnPointY(cnt);

				cnt++;

				if (m_map[dstX][dstY].terrain != TerrainType::Obstacle && m_map[pAgent->x][pAgent->y].currentGridAgent == nullptr)
				{
					Agent* childAgent = new Agent(Gene::BreedAndMutate(pAgent->m_gene, pAgent->m_gene), pAgent->species);
					AddAgent(*childAgent);
					pAgent->mateCount++;
					break;
				}
			}
		}

		// You DEAD and become foods
		if (pAgent->currentHealth <= 0)
		{
			// Drop held item
			int cnt = 0;

			if (pAgent->handObject != ObjectType::None)
			{
				while (cnt < 25)
				{
					int dstX = pAgent->x + getSpawnPointX(cnt);
					int dstY = pAgent->y + getSpawnPointY(cnt);

					cnt++;

					if (m_map[dstX][dstY].terrain != TerrainType::Obstacle && m_map[dstX][dstY].object == ObjectType::None)
					{
						m_map[dstX][dstY].object = pAgent->handObject;
						break;
					}
				}

				pAgent->handObject = ObjectType::None;
			}

			// Spawn foods
			int foodCount = 3; // TODO: based on agent VIT

			for (int i = 0; i < foodCount; i++)
			{
				while (cnt < 25)
				{
					int dstX = pAgent->x + getSpawnPointX(cnt);
					int dstY = pAgent->y + getSpawnPointY(cnt);

					cnt++;

					if (m_map[dstX][dstY].terrain != TerrainType::Obstacle && m_map[dstX][dstY].object == ObjectType::None)
					{
						m_map[dstX][dstY].object = ObjectType::Food;
						break;
					}
				}
			}

			// Remove agent
			Agent* deadAgent = &(*pAgent);
			pAgent = m_agentList.erase(pAgent);
			delete deadAgent;

			continue;
		}

		// Set agent to map
		m_map[pAgent->x][pAgent->y].agentSpecies = pAgent->species;
		m_map[pAgent->x][pAgent->y].currentGridAgent = &(*pAgent);
		
		if (pAgent->talking)
		{
			m_map[pAgent->x][pAgent->y].hasAgentTalking = true;
		}

		// Goto next agent
		++pAgent;
	}
	#pragma endregion

	#pragma region KeepAgentMinimumCount
	int aCount = m_agentList.size();
	for (int i = 0; i < MINIMAL_AGENT_COUNT - aCount; i++)
	{
		Agent& randomAgent = m_agentList.at(rand() % aCount);
		Agent* agent = new Agent(Gene::BreedAndMutate(randomAgent.m_gene, randomAgent.m_gene), randomAgent.species);
		
		agent->x = rand() % MAPSIZE_X;
		agent->y = rand() % MAPSIZE_Y;

		while (m_map[agent->x][agent->y].terrain == TerrainType::Obstacle || m_map[agent->x][agent->y].currentGridAgent != nullptr)
		{
			agent->x = rand() % MAPSIZE_X;
			agent->y = rand() % MAPSIZE_Y;
		}

		AddAgent(*agent);
	}
	#pragma endregion

	time++;
}

void SandboxBackend::Render()
{
}

void SandboxBackend::AddAgent(Agent & agent)
{
	m_agentList.push_back(agent);
	m_map[agent.x][agent.y].agentSpecies = agent.species;
	m_map[agent.x][agent.y].currentGridAgent = &agent;
}

bool SandboxBackend::UseItem(Agent & agent, ObjectType item)
{
	if (item == ObjectType::None || item == ObjectType::AgentSaying)
	{
		return false;
	}

	switch (item)
	{
	// Food, restores stamina.
	case ObjectType::Food:
		agent.currentStamina += 40;
		if (agent.currentStamina > agent.maxStamina)
		{
			agent.currentStamina = agent.maxStamina;
		}
		return true;
		break;

	// Seed, can be used on dirt
	case ObjectType::Seed:
		if (m_map[agent.x][agent.y].terrain == TerrainType::Dirt)
		{
			// Seed the dirt
			m_map[agent.x][agent.y].terrain = TerrainType::SeedDirt;

			// Set seed timer to 30 ~ 50, and produce food when timer goes to 0.
			m_map[agent.x][agent.y].terrainMeta = 30.0 + (float)(rand() % 20);
			return true;
		}
		return false;
		break;

	// Stone, can be used to open nuts or attack front target
	case ObjectType::Stone:
		if (m_map[agent.x][agent.y].object == ObjectType::Nut)
		{
			m_map[agent.x][agent.y].object = ObjectType::None;

			agent.currentStamina += 100;
			if (agent.currentStamina > agent.maxStamina)
			{
				agent.currentStamina = agent.maxStamina;
			}

			agent.currentHealth += 3;
			if (agent.currentHealth > agent.maxHealth)
			{
				agent.currentHealth = agent.maxHealth;
			}

			return true;
		}

		int newX = agent.x + directionVector[agent.faceDirection][0],
			newY = agent.y + directionVector[agent.faceDirection][1];

		// Boundary check
		if (newX < 0 || newX > MAPSIZE_X || newY < 0 || newY > MAPSIZE_Y)
		{
			return false;
		}

		Agent* attackTarget = m_map[newX][newY].currentGridAgent;

		if (attackTarget != nullptr)
		{
			// we damaged our target!
			attackTarget->currentHealth -= 3;
			return true;
		}
		return false;
		break;

	// It cannot be used.
	case ObjectType::Nut:
		return false;
	}
}

int SandboxBackend::getSpawnPointX(int index)
{
	int lenth = 1, walkLenth = 0, turnCount = 0, cDirc = 0;
	int dirc[4] = { 1, 0, -1, 0 };
	int x = 0;
	for (int i = 0; i < index; i++)
	{
		x += dirc[cDirc];
		walkLenth++;

		if (walkLenth >= lenth)
		{
			turnCount++;
			cDirc++;
			cDirc %= 4;

			if (turnCount % 2 == 0)
			{
				lenth += 1;
			}

			walkLenth = 0;
		}
	}

	return x;
}

int SandboxBackend::getSpawnPointY(int index)
{
	int lenth = 1, walkLenth = 0, turnCount = 0, cDirc = 0;
	int dirc[4] = { 0, 1, 0, -1 };
	int y = 0;
	for (int i = 0; i < index; i++)
	{
		y += dirc[cDirc];
		walkLenth++;

		if (walkLenth >= lenth)
		{
			turnCount++;
			cDirc++;
			cDirc %= 4;

			if (turnCount % 2 == 0)
			{
				lenth += 1;
			}

			walkLenth = 0;
		}
	}

	return y;
}
