#include "GraphNet.h"

GraphNet::GraphNet(Gene & gene) : 
	m_gene(gene)
{
	// Create the graph.

	// Add input and output nodes.
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		Gene::VertexNode nd;
		
		nd.innovationNum = 0;

		nd.index = i;
		nd.nodeType = Gene::NodeType::Input;

		nd.act = Gene::Activation::Identity;
		nd.initialBias = 0;
		nd.learningDecay = 0.0f;

		vertices.push_back(nd);
	}

	for (int i = 0; i < OUTPUT_COUNT; i++)
	{
		Gene::VertexNode nd;

		nd.innovationNum = 0;

		nd.index = INPUT_COUNT + i;
		nd.nodeType = Gene::NodeType::Output;

		nd.act = Gene::Activation::Identity;
		nd.initialBias = 0;
		nd.learningDecay = 0.0f;

		vertices.push_back(nd);
	}

	// Readin gene nodes by copy them
	vertices.insert(vertices.end(), gene.vertices.begin(), gene.vertices.end());
	edges = gene.edges;
}

GraphNet::~GraphNet()
{
}

GraphNet::Output GraphNet::Forward(InputSignal signal)
{
	return Forward_ST_CPU(signal);
}

GraphNet::Output GraphNet::Forward_ST_CPU(InputSignal& signal)
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
			vertices.at(edges.at(i).at(j).endVertex).
		}
	}

	return Output();
}
