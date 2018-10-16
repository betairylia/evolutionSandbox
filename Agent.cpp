#include "Agent.h"

Agent::Agent(Gene _gene) : 
	m_gene(_gene),
	m_brain(m_gene)
{
}

Agent::~Agent()
{
}

void Agent::AgentTick()
{
}

void Agent::DoAction()
{
}
