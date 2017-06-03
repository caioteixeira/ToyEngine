#pragma once
#include <string>
#include <SDL.h>

inline void ThrowIfFailed(HRESULT hr, char* str = nullptr)
{
	if(hr != S_OK)
	{
		if(str != nullptr)
		{
			SDL_Log(str, SDL_LOG_CATEGORY_ERROR);
		}

		//TODO: Create DXException
		throw 1;
	}
}
