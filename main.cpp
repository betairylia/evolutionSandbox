#include <iostream>
#include <SDL.h>

#include "EvolutionSandbox.h"

using namespace std;

int main(int argc, char * argv[])
{
	SDLApplication* mainApp = new EvolutionSandbox();

	return mainApp->OnExecute();
}