#pragma once

#include <SDL.h>

class SDLApplication
{
protected:
	bool m_isRunning;

public:
	SDLApplication();
	~SDLApplication();

	int OnExecute();
	virtual bool OnInit();
	virtual void OnEvent(SDL_Event* evt);
	virtual void OnLoop();
	virtual void OnRender();
	virtual void OnCleanup();
};

