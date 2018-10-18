#pragma once

#include "utils.h"

#include <vector>
#include <random>
#include <iostream>

class Gene
{
public:

	Gene();
	Gene(Gene& g);
	~Gene();

	std::vector< VertexNode > vertices;
	std::vector< std::vector< EdgeNode > > edges;

	static Gene& BreedAndMutate(Gene& a, Gene& b);

	std::random_device* rd;
	std::mt19937* gen;
	std::normal_distribution<float>* normal_dist;
	std::normal_distribution<float>* big_normal_dist;
};

