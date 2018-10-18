#include "Gene.h"

Gene::Gene()
{
	// Init RNG
	rd = new std::random_device();
	gen = new std::mt19937((*rd)());
	normal_dist = new std::normal_distribution<float>(0.0f, 1.0f);

	VertexNode n;
	
	// TODO: something with innovation num
	n.innovationNum = 0;

	n.index = IO_COUNT + vertices.size();
	n.act = Activation::ReLU;
	n.bias = (*normal_dist)(*gen);

	// Fixed values
	n.nodeType = NodeType::Hidden;
	n.learningDecay = 0.9f;
	n.valueDecay = 0.6f;
	n.preActivation = 0.0f;
	n.beforeActValue = 0.0f;
	n.value = 0.0f;

	vertices.push_back(n);

	// Create edges
	for (int i = 0; i < IO_COUNT; i++)
	{
		edges.push_back(*(new std::vector<EdgeNode>));
	}
	edges.push_back(*(new std::vector<EdgeNode>));
	
	for (int i = 0; i < INPUT_COUNT; i++)
	{
		EdgeNode e;

		// TODO: something with innovation num
		e.innovationNum = 0;

		e.startVertex = i;
		e.endVertex = IO_COUNT; // To hidden node 1
		e.enabled = true;

		e.weight = 1 * (*normal_dist)(*gen);

		edges.at(i).push_back(e);
	}

	for (int i = 0; i < OUTPUT_COUNT; i++)
	{
		EdgeNode e;

		// TODO: something with innovation num
		e.innovationNum = 0;

		e.startVertex = IO_COUNT; // Start from hidden 1
		e.endVertex = INPUT_COUNT + i; // To output node
		e.enabled = true;

		e.weight = 1 * (*normal_dist)(*gen);

		edges.at(IO_COUNT).push_back(e);
	}
}

Gene::Gene(Gene & g)
{
	// Init RNG
	rd = new std::random_device();
	gen = new std::mt19937((*rd)());
	normal_dist = new std::normal_distribution<float>(0.0f, 1.0f);

	vertices = g.vertices;
	edges = g.edges;
}

Gene::~Gene()
{
}

Gene & Gene::BreedAndMutate(Gene & a, Gene & b)
{
	//std::cout << "Breeding and mutating..." << std::endl;

	// Currently use only one gene
	Gene* gene = new Gene(a);

	int len = gene->vertices.size();

	// Randomly create new node in serial
	for (int _i = 0; _i < len; _i++)
	{
		int i = _i + IO_COUNT;
		std::vector<bool> hasEdge(len + IO_COUNT);

		int eLen = gene->edges.at(i).size();
		for (int j = 0; j < eLen; j++)
		{
			hasEdge.at(gene->edges.at(i).at(j).endVertex) = true;
			// Create a new node
			if (rand() % 100 < 1)
			{
				VertexNode n;

				// TODO: something with innovation num
				n.innovationNum = 0;

				n.index = IO_COUNT + gene->vertices.size();
				n.act = Activation::ReLU;
				n.bias = (*gene->normal_dist)(*gene->gen);

				// Fixed values
				n.nodeType = NodeType::Hidden;
				n.learningDecay = 0.9f;
				n.valueDecay = 0.6f;
				n.preActivation = 0.0f;
				n.beforeActValue = 0.0f;
				n.value = 0.0f;

				gene->vertices.push_back(n);
				
				///////////////////////////////////////////////////////////

				// Create new edge: start -> node
				EdgeNode e;

				// Create edge table for the new node
				gene->edges.push_back(*(new std::vector<EdgeNode>));

				// Disable original edge
				gene->edges.at(i).at(j).enabled = false;
				
				// TODO: something with innovation num
				e.innovationNum = 0;

				e.startVertex = i; // Start from hidden 1
				e.endVertex = n.index; // To output node
				e.enabled = true;

				// TODO: fixed initial value ?
				e.weight = 1 * (*gene->normal_dist)(*gene->gen);

				// Add it to the list
				gene->edges.at(i).push_back(e);

				// Create new edge: node -> end
				// TODO: something with innovation num
				e.innovationNum = 0;

				e.startVertex = n.index; // Start from hidden 1
				e.endVertex = gene->edges.at(i).at(j).endVertex; // To output node
				e.enabled = true;

				e.weight = 1 * (*gene->normal_dist)(*gene->gen);

				// Add it to the list
				gene->edges.at(n.index).push_back(e);
			}
		}

		// Randomly create new edges
		if (rand() % 100 < 25)
		{
			int targetId = rand() % len;
			// You are not supposed to create a edge back to input.
			if (targetId > INPUT_COUNT && !hasEdge.at(targetId))
			{
				// Create the edge (even for a self-connection)
				EdgeNode e;

				// TODO: something with innovation num
				e.innovationNum = 0;

				e.startVertex = i;
				e.endVertex = targetId;
				e.enabled = true;

				e.weight = 1 * (*gene->normal_dist)(*gene->gen);

				// Add it to the list
				gene->edges.at(i).push_back(e);
			}
		}
	}

	for (int i = 0; i < len; i++)
	{
		gene->vertices.at(i).bias += 0.04 * (*gene->normal_dist)(*gene->gen);

		// Randomly reassign node bias
		if (rand() % 100 < 5)
		{
			gene->vertices.at(i).bias += (*gene->normal_dist)(*gene->gen);
		}

		int eLen = gene->edges.at(i).size();
		for (int j = 0; j < eLen; j++)
		{
			gene->edges.at(i).at(j).weight += 0.01 * (*gene->normal_dist)(*gene->gen);

			// Randomly reassign edge weight
			if (rand() % 100 < 5)
			{
				gene->edges.at(i).at(j).weight += 0.3 * (*gene->normal_dist)(*gene->gen);
			}
		}
	}

	return *(gene);
}
