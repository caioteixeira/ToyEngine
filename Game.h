#pragma once

class Game
{
public:
	Game();
	~Game();
	bool Init();
	void RunLoop();
	void Quit();

private:
	void StartGame();

	bool mShouldQuit;
};

