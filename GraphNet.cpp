#include "GraphNet.h"

GraphNet::GraphNet(Gene & gene) : 
	m_gene(gene)
{
	// Create the graph.

	// Add input and output nodes.
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		VertexNode nd;
		
		nd.innovationNum = 0;

		nd.index = i;
		nd.nodeType = NodeType::InputNeuron;

		nd.act = Activation::Identity;
		nd.bias = 0;
		nd.learningDecay = 0.0f;
		nd.valueDecay = 0.0f;
		nd.preActivation = 0.0f;
		nd.beforeActValue = 0.0f;
		nd.value = 0.0f;

		vertices.push_back(nd);
	}

	for (int i = 0; i < OUTPUT_COUNT; i++)
	{
		VertexNode nd;

		nd.innovationNum = 0;

		nd.index = INPUT_COUNT + i;
		nd.nodeType = NodeType::OutputNeuron;

		nd.act = Activation::Identity;
		nd.bias = 0;
		nd.learningDecay = 0.0f;
		nd.valueDecay = 0.0f;
		nd.preActivation = 0.0f;
		nd.beforeActValue = 0.0f;
		nd.value = 0.0f;

		vertices.push_back(nd);
	}

	// Readin gene nodes by copy them
	vertices.insert(vertices.end(), gene.vertices.begin(), gene.vertices.end());
	edges = gene.edges; // <-- this is a copy process.
}

GraphNet::~GraphNet()
{
	delete &m_gene;
}

AgentOutput GraphNet::Forward(InputSignal& signal)
{
	return Forward_ST_CPU(signal);
}

AgentOutput GraphNet::Forward_ST_CPU(InputSignal& signal)
{
	// Feed in input data
	// vision
	for (int i = 0; i < VISION_DEPTH * VISION_WIDTH * VISION_CHANNEL; i++)
	{
		vertices.at(i).value = signal.vision[i];
	}

	// touch
	for (int i = 0; i < TOUCH_DEPTH; i++)
	{
		vertices.at(VISION_DEPTH * VISION_WIDTH * VISION_CHANNEL + i).value = signal.touch[i];
	}

	// status
	for (int i = 0; i < STATUS_DEPTH; i++)
	{
		vertices.at(VISION_DEPTH * VISION_WIDTH * VISION_CHANNEL + TOUCH_DEPTH + i).value = signal.status[i];
	}

	// Calculate each node's pre-activation
	int len = vertices.size();
	for (int i = 0; i < len; i++)
	{
		int eLen = edges.at(i).size();
		for (int j = 0; j < eLen; j++)
		{
			if (edges.at(i).at(j).enabled)
			{
				vertices.at(edges.at(i).at(j).endVertex).preActivation += vertices.at(i).value * edges.at(i).at(j).weight;
			}
		}
	}

	// Update node value
	for (int i = 0; i < len; i++)
	{
		vertices.at(i).beforeActValue *= vertices.at(i).valueDecay;
		vertices.at(i).beforeActValue += vertices.at(i).preActivation;
		vertices.at(i).value = vertices.at(i).beforeActValue + vertices.at(i).bias + 0.1f * (*m_gene.normal_dist)(*m_gene.gen); // Add a gaussian noise
		switch (vertices.at(i).act)
		{
			case Activation::ReLU:
				vertices.at(i).value = (vertices.at(i).value > 0) ? vertices.at(i).value : 0;
				break;

			case Activation::Identity:
				vertices.at(i).value = vertices.at(i).value;
		}
	}

	AgentOutput output;

	// Check outputs via max-out
	float max_v = -9999999;
	int max_idx = -1;

	for (int i = INPUT_COUNT; i < INPUT_COUNT + 4; i++)
	{
		if (max_v < vertices.at(i).value)
		{
			max_v = vertices.at(i).value;
			max_idx = i - INPUT_COUNT;
		}
	}

	if (max_idx == -1) { max_idx = 0; }
	output.direction = (OutputDirection)max_idx;

	max_v = -9999999;
	max_idx = -1;
	for (int i = INPUT_COUNT + 4; i < IO_COUNT; i++)
	{
		if (max_v < vertices.at(i).value)
		{
			max_v = vertices.at(i).value;
			max_idx = i - INPUT_COUNT - 4;
		}
	}

	if (max_idx == -1) { max_idx = 0; }
	output.action = (OutputAction)max_idx;

	return output;
}
