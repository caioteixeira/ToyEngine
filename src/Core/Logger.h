#pragma once

namespace Engine::Logger
{
	void DebugLog(char *format, ...);

	void DebugLogError(char *format, ...);

	void DebugLogWarning(char *format, ...);
}