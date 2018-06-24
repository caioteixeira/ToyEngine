#include "Input.h"

bool Input::IsKeyDown(KeyCode key)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    return state[key];
}

bool Input::IsRightMouseButtonDown()
{
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT)) 
    {
        return true;
    }
    return false;
}

void Input::GetMousePosition(int& x, int& y)
{
    SDL_GetMouseState(&x, &y);
}
