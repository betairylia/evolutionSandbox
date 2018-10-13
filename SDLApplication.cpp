#include "SDLApplication.h"

SDLApplication::SDLApplication()
{
	m_isRunning = true;
}

SDLApplication::~SDLApplication()
{
}

int SDLApplication::OnExecute()
{
	if (OnInit() == false) 
	{
		return -1;
	}

	SDL_Event evt;

	while (m_isRunning) 
	{
		while (SDL_PollEvent(&evt)) 
		{
			OnEvent(&evt);
		}

		OnLoop();
		OnRender();
	}

	OnCleanup();

	return 0;
}

bool SDLApplication::OnInit()
{
	// Init complete.
	return true;
}

void SDLApplication::OnEvent(SDL_Event * evt)
{
}

void SDLApplication::OnLoop()
{
}

void SDLApplication::OnRender()
{
}

void SDLApplication::OnCleanup()
{
}
