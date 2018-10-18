#include "Agent.h"

int Agent::stepPerTick = 10;

Agent::Agent(Gene& _gene, int _species) : 
	m_gene(_gene),
	m_brain((*new GraphNet(_gene))),
	species(_species)
{
	// TODO: change me!
	currentHealth = prevHealth = maxHealth = 10;
	currentStamina = prevStamina = maxStamina = 25 + rand() % 10;
	staminaMultiplier = 1;
	lifetime = 0;
	mateCount = 0;
	handObject = ObjectType::None;
	talking = false;
	x = 0;
	y = 0;

	// and me!
	actionTickCount = 1;
	_tickCount = 0;
}

Agent::~Agent()
{
	delete &m_brain;
}

void Agent::AgentTick(InputSignal inputSignal)
{
	for (int i = 0; i < stepPerTick; i++)
	{
		// inputSignal is a copy so it is safe to modify it
		inputSignal.status[STATUS_DEPTH - 1] = (i > (stepPerTick / 2)) ? 1 : 0;
		m_actionCache.push_back(m_brain.Forward(inputSignal));
	}
}

AgentOutput Agent::DoAction()
{
	// Pick a random action in current cache
	int id = rand() % m_actionCache.size();
	AgentOutput result = m_actionCache.at(id);

	// Clear the cache
	m_actionCache.clear();

	_tickCount = 0;

	return result;
}

bool Agent::useStamina(float stamina)
{
	stamina = stamina * staminaMultiplier;
	
	if (currentStamina >= stamina)
	{
		currentStamina -= stamina;
		return true;
	}

	return false;
}
