#pragma once
#include <iostream>

#include "SDL_image.h"

#include "SDLApplication.h"
#include "SandboxBackend.h"
#include "LTexture.h"

using namespace std;

class EvolutionSandbox :
	public SDLApplication
{
protected:
	SDL_Window* mp_window;
	SDL_Renderer* mp_Renderer;
	SDL_Surface* mp_screenSurface;

	int m_screenWidth = 1440, m_screenHeight = 900;
	SandboxBackend* sandbox;

	LTexture mainTex;

public:
	EvolutionSandbox();
	~EvolutionSandbox();

	virtual bool OnInit();
	virtual void OnEvent(SDL_Event* evt);
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
};

