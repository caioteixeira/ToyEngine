#include "Logger.h"
#include "../Math.h"

void Logger::DebugLog(const char* format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    SDL_Log(format, arglist);
}

void Logger::DebugLogError(const char* format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    SDL_Log(format, SDL_LOG_CATEGORY_ERROR, arglist);
}

void Logger::LogMatrix(const DirectX::SimpleMath::Matrix matrix)
{
    DebugLog("%f %f %f %f", matrix(0, 0), matrix(0, 1), matrix(0, 2), matrix(0, 3));
    DebugLog("%f %f %f %f", matrix(1, 0), matrix(1, 1), matrix(1, 2), matrix(1, 3));
    DebugLog("%f %f %f %f", matrix(2, 0), matrix(2, 1), matrix(2, 2), matrix(2, 3));
    DebugLog("%f %f %f %f", matrix(3, 0), matrix(3, 1), matrix(3, 2), matrix(3, 3));
}
