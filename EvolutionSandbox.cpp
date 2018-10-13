#include "EvolutionSandbox.h"

EvolutionSandbox::EvolutionSandbox()
{
}

EvolutionSandbox::~EvolutionSandbox()
{
}

bool EvolutionSandbox::OnInit()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL initialization failed. SDL Error: " << SDL_GetError();
		return false;
	}
	
	// Create window
	mp_window = NULL;
	mp_window = SDL_CreateWindow("Evolution Sandbox", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_screenWidth, m_screenHeight, SDL_WINDOW_SHOWN);
	if(mp_window == NULL)
	{
		cout << "Window creation failed. SDL Error: " << SDL_GetError();
		return false;
	}

	// Get window surface
	mp_screenSurface = SDL_GetWindowSurface(mp_window);

	//Fill the surface white
	SDL_FillRect(mp_screenSurface, NULL, SDL_MapRGB(mp_screenSurface->format, 0xFF, 0xCC, 0x77));

	//Update the surface
	SDL_UpdateWindowSurface(mp_window);

	// Init complete.
	return true;
}

void EvolutionSandbox::OnEvent(SDL_Event * evt)
{
}

void EvolutionSandbox::OnLoop()
{
}

void EvolutionSandbox::OnRender()
{
}

void EvolutionSandbox::OnCleanup()
{
}
