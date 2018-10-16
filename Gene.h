#pragma once
class Gene
{
public:
	enum NodeType
	{
		Hidden,
		Error,

		// Do not use them in a gene ( it should be added automatically when build the graph )
		Input,
		Output,
	};

	enum Activation
	{
		Identity,
		ReLU
	};

	struct VertexNode
	{
		int innovationNum;
		
		int index;			 // Index of this vertex
		NodeType nodeType;	 // Node type (hidden / error)

		Activation act;		 // Activation function
		float beforeActValue;
		float bias;			 // Initial bias ( only when preActivation higher than this, output of the node will be activated as preActivation - bias ( biased ReLU ) )
		float learningDecay; // How much gradients will it back-propagate
		float valueDecay;	 // fixed node value decay = 0.6 ( value decay after each forward step )
		
		float preActivation; // For computation
		float value;		 // For computation
	};

	struct EdgeNode
	{
		int innovationNum;
		
		int startVertex;	 // vertexIndex < IO_Count = I/O nodes. 0 ~ 159 = Input nodes, 160 ~ 169 = Output nodes.
		int endVertex;
		bool enabled;		 // Edges with low weights will become DISABLED with high prob. and DISABLED edge will be randomly removed, kept, or re-enabled.

		float weight;		 // Initial weight value.
		// float learningRate;
	};

	Gene();
	~Gene();

	std::vector< Gene::VertexNode > vertices;
	std::vector< std::vector< Gene::EdgeNode > > edges;

	static Gene& BreedAndMutate(Gene& a, Gene& b);
};

