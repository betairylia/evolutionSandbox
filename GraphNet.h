#pragma once

#include <string>
#include <vector>

#include "Gene.h"

class GraphNet
{
public:
	GraphNet(Gene& gene);
	~GraphNet();

	AgentOutput Forward(InputSignal& signal); // Push the net forward for 1 step.

	// Different ways to forward
	AgentOutput Forward_ST_CPU(InputSignal& signal);

	Gene& m_gene;

private:
	std::vector< VertexNode > vertices;
	std::vector< std::vector< EdgeNode > > edges;
	// Edges array: [from input nodes] [empty for output nodes] [from hidden nodes]
};

