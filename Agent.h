#pragma once

#include <cstdlib>
#include <string>

#include "GraphNet.h"

class Agent
{
public:
	Agent(Gene& _gene, int _species);
	~Agent();

	void AgentTick(InputSignal inputSignal); // evaluate k steps in a tick, Sum and collect the actions.
	AgentOutput DoAction(); // Do the selected action and clear action cache.
	bool useStamina(float stamina);

	std::vector<AgentOutput> m_actionCache;

	static int stepPerTick;
	int actionTickCount, _tickCount;

	int x, y;
	OutputDirection faceDirection;
	int species;
	float currentHealth, prevHealth, maxHealth;
	float currentStamina, prevStamina, maxStamina;
	float staminaMultiplier;
	float lifetime;
	int mateCount;
	ObjectType handObject;

	bool talking;

	GraphNet& m_brain;
	Gene& m_gene;
};

