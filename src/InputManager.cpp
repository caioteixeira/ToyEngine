#include "InputManager.h"
#include "WindowsHeaders.h"
#include "Game.h"
#include "Core/imgui/imgui.h"
#include "Core/imgui/imgui_impl_dx12.h"

InputManager::InputManager(Game& game)
	: mGame(game)
{
}

InputManager::~InputManager()
{
}

LRESULT InputManager::ProcessInput(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(ImGui_ImplDX12_WndProcHandler(hwnd, msg, wParam, lParam))
	{
		return true;
	}
}

/*
void InputManager::ProcessInput()
{
	ImGuiIO& io = ImGui::GetIO();

	// Poll events from SDL
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			mGame.QuitGame();
			break;
		case SDL_MOUSEWHEEL:
		{
			float wheelValue = 0;

			if (event.wheel.y > 0)
			{
				wheelValue = 1;
			}
			else if (event.wheel.y < 0)
			{
				wheelValue = -1;
			}

			io.MouseWheel = wheelValue;
			break;
		}

		case SDL_MOUSEBUTTONDOWN:
		{
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				io.MouseDown[0] = true;
				break;
			case SDL_BUTTON_RIGHT:
				io.MouseDown[1] = true;
				break;
			case SDL_BUTTON_MIDDLE:
				io.MouseDown[3] = true;
				break;
			default:
				break;
			}
			break;
		}

		case SDL_MOUSEBUTTONUP:
		{
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				io.MouseDown[0] = false;
				break;
			case SDL_BUTTON_RIGHT:
				io.MouseDown[1] = false;
				break;
			case SDL_BUTTON_MIDDLE:
				io.MouseDown[3] = false;
				break;
			}
			break;
		}

		case SDL_MOUSEMOTION:
		{
			io.MousePos.x = static_cast<float>(event.motion.x);
			io.MousePos.y = static_cast<float>(event.motion.y);
			break;
		}

		case SDL_TEXTINPUT:
		{
			io.AddInputCharactersUTF8(event.text.text);
			break;
		}
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			int key = event.key.keysym.sym & ~SDLK_SCANCODE_MASK;
			io.KeysDown[key] = (event.type == SDL_KEYDOWN);
			io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
			io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
			io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			break;
		}
		default:
			// Ignore other events
			break;
		}
	}
	
}
*/

void InputManager::Initialize()
{
	//TODO: Implement actions and stuff
}
