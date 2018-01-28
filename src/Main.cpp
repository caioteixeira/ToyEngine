#include "Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	Game game(hInstance);

	if(game.Init())
	{
		game.RunLoop();
	}

	return 0;
}
