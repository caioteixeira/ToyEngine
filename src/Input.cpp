#include "Input.h"

bool Input::IsKeyDown(KeyCode key)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    return state[key];
}
