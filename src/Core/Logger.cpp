#include "Logger.h"
#include "../WindowsHeaders.h"

void Logger::DebugLog(char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	SDL_Log(format, arglist);
}

void Logger::DebugLogError(char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
	SDL_Log(format, SDL_LOG_CATEGORY_ERROR, arglist);
}