#pragma once

#include <string>
#include <vector>

#include "Gene.h"

// TODO: strides in vision & offset inside ticks?
constexpr auto VISION_WIDTH = 7;
constexpr auto VISION_DEPTH = 7;
constexpr auto VISION_CHANNEL = 3;
constexpr auto TOUCH_DEPTH = 5;
constexpr auto STATUS_DEPTH = 8;
constexpr auto INPUT_COUNT = VISION_DEPTH * VISION_WIDTH * VISION_CHANNEL + TOUCH_DEPTH + STATUS_DEPTH;

constexpr auto OUTPUT_DIRC_DEPTH = 4;
constexpr auto OUTPUT_ACTION_DEPTH = 6;
constexpr auto OUTPUT_COUNT = OUTPUT_DIRC_DEPTH + OUTPUT_ACTION_DEPTH;

constexpr auto IO_COUNT = INPUT_COUNT + OUTPUT_COUNT;

class GraphNet
{
public:
	struct InputSignal 
	{
		float vision[VISION_WIDTH * VISION_DEPTH * VISION_CHANNEL]; // 7x7x3, Terrain - Species - Objects & Information
		float touch[TOUCH_DEPTH]; // front has object - front object - hand has object - hand object - current terrain
		float status[STATUS_DEPTH]; // health% - low health - health cost last frame - stamina% - low stamina - stamina cost last frame - mateable - tick clock
	};

	enum OutputDirection
	{
		Up,
		Down,
		Left,
		Right
	};

	enum OutputAction
	{
		Move,
		Run,
		Stay,
		Say,
		Attack_Use,
		Pick_Drop,
	};

	struct Output
	{
		OutputAction action;
		OutputDirection direction;
	};

	GraphNet(Gene& gene);
	~GraphNet();

	Output Forward(InputSignal signal); // Push the net forward for 1 step.

	// Different ways to forward
	Output Forward_ST_CPU(InputSignal& signal);

	Gene& m_gene;

private:
	std::vector< Gene::VertexNode > vertices;
	std::vector< std::vector< Gene::EdgeNode > > edges;
	// Edges array: [from input nodes] [empty for output nodes] [from hidden nodes]
};

