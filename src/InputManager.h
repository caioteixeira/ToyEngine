#pragma once


namespace Engine
{
	class Game;

	class InputManager
	{
	public:
		InputManager(Game& game);
		~InputManager();
		void ProcessInput();
		void Initialize();
	private:
		Game & mGame;
	};
}


