#pragma once
#include <string>
#include "../../Core/Logger.h"

inline void ThrowIfFailed(HRESULT hr, char* str = nullptr)
{
	if(hr != S_OK)
	{
		if(str != nullptr)
		{
			Logger::Log(str);
		}

		//TODO: Create DXException
		throw 1;
	}
}
