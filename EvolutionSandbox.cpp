#include "EvolutionSandbox.h"

EvolutionSandbox::EvolutionSandbox()
{
}

EvolutionSandbox::~EvolutionSandbox()
{
}

bool EvolutionSandbox::OnInit()
{
	// Create a sandbox backend
	sandbox = new SandboxBackend(1234);
	sandbox->Init();

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL initialization failed. SDL Error: " << SDL_GetError() << endl;
		return false;
	}
	
	// Create window
	mp_window = NULL;
	mp_window = SDL_CreateWindow("Evolution Sandbox", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_screenWidth, m_screenHeight, SDL_WINDOW_SHOWN);
	if(mp_window == NULL)
	{
		cout << "Window creation failed. SDL Error: " << SDL_GetError() << endl;
		return false;
	}

	mp_Renderer = NULL;
	mp_Renderer = SDL_CreateRenderer(mp_window, -1, SDL_RENDERER_ACCELERATED);
	if (mp_Renderer == NULL)
	{
		cout << "Renderer creation failed. SDL Error: " << SDL_GetError() << endl;
		return false;
	}

	// Get window surface
	mp_screenSurface = SDL_GetWindowSurface(mp_window);

	//Fill the surface white
	SDL_FillRect(mp_screenSurface, NULL, SDL_MapRGB(mp_screenSurface->format, 0xFF, 0xCC, 0x77));

	//Update the surface
	SDL_UpdateWindowSurface(mp_window);

	// Init SDL_Images
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << endl;
		return false;
	}

	mainTex.gRenderer = mp_Renderer;
	mainTex.gWindow = mp_window;

	if (!mainTex.loadFromFile("tiles.png"))
	{
		cout << "Failed to load main texture!" << endl;
		return false;
	}

	// Init complete.
	return true;
}

void EvolutionSandbox::OnEvent(SDL_Event * evt)
{
}

void EvolutionSandbox::OnLoop()
{
	cout << "Max Life = " << sandbox->maxLifeSpan << endl;
	sandbox->Update();
}

void EvolutionSandbox::OnRender()
{
	// Clear screen buffer
	SDL_SetRenderDrawColor(mp_Renderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderClear(mp_Renderer);

	// Draw terrain & Objects
	for (int x = 0; x < MAPSIZE_X; x++)
	{
		for (int y = 0; y < MAPSIZE_Y; y++)
		{
			mainTex.render(x * 12, y * 12, &spriteClips_Terrain[sandbox->m_map[x][y].terrain]);

			// Draw Objects
			if (sandbox->m_map[x][y].object != ObjectType::None)
			{
				mainTex.render(x * 12, y * 12, &spriteClips_Objects[sandbox->m_map[x][y].object - 1]);
			}
		}
	}

	int len = sandbox->m_agentList.size();
	for (int i = 0; i < len; i++)
	{
		mainTex.render(sandbox->m_agentList.at(i)->x * 12, sandbox->m_agentList.at(i)->y * 12, &spriteClips_Species[sandbox->m_agentList.at(i)->species - 1]);
	}

	// Flush screen buffer
	SDL_RenderPresent(mp_Renderer);
}

void EvolutionSandbox::OnCleanup()
{
}
