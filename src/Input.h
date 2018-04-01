#pragma once
#include "WindowsHeaders.h"

namespace Engine
{
    enum KeyCode : int
    {
        A = SDL_SCANCODE_A,
        B = SDL_SCANCODE_B,
        C = SDL_SCANCODE_C,
        D = SDL_SCANCODE_D,
        E = SDL_SCANCODE_E,
        F = SDL_SCANCODE_F,
        G = SDL_SCANCODE_G,
        H = SDL_SCANCODE_H,
        J = SDL_SCANCODE_J,
        I = SDL_SCANCODE_I,
        K = SDL_SCANCODE_K,
        L = SDL_SCANCODE_L,
        M = SDL_SCANCODE_M,
        N = SDL_SCANCODE_N,
        O = SDL_SCANCODE_O,
        P = SDL_SCANCODE_P,
        Q = SDL_SCANCODE_Q,
        R = SDL_SCANCODE_R,
        S = SDL_SCANCODE_S,
        T = SDL_SCANCODE_T,
        U = SDL_SCANCODE_U,
        V = SDL_SCANCODE_V,
        W = SDL_SCANCODE_W,
        X = SDL_SCANCODE_X,
        Y = SDL_SCANCODE_Y,
        Z = SDL_SCANCODE_Z,
        Zero = SDL_SCANCODE_0,
        One = SDL_SCANCODE_1,
        Two = SDL_SCANCODE_2,
        Three = SDL_SCANCODE_3,
        Four = SDL_SCANCODE_4,
        Five = SDL_SCANCODE_5,
        Six = SDL_SCANCODE_6,
        Seven = SDL_SCANCODE_7,
        Eight = SDL_SCANCODE_8,
        Nine = SDL_SCANCODE_9,
        Space = SDL_SCANCODE_SPACE,
        LeftShift = SDL_SCANCODE_LSHIFT,
        RightShift = SDL_SCANCODE_RSHIFT,
        LeftCtrl = SDL_SCANCODE_LCTRL,
        RightCtrl = SDL_SCANCODE_RCTRL,
        LeftAlt = SDL_SCANCODE_LALT,
        RightAlt = SDL_SCANCODE_RALT
    };

    class Input
    {
    public:
        static bool IsKeyDown(KeyCode key);
    };
}