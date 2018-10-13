#pragma once
#include <SDL.h>

#include "Agent.h"
#include "GraphNet.h"

class SandboxBackend
{
public:
	SandboxBackend();
	~SandboxBackend();

	bool Init();
	void Update();
	void Render();
};

