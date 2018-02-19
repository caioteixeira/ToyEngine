#include "ConfigSystem.h"

using namespace Engine;

std::unordered_map<std::string, std::string> ConfigSystem::mStringMap = {};
std::unordered_map<std::string, int> ConfigSystem::mIntMap = {};
std::unordered_map<std::string, double> ConfigSystem::mDoubleMap = {};


void ConfigSystem::Init()
{
	//TODO: Load configVars.json
}

std::string ConfigSystem::GetStringValue(std::string key)
{
	if (mStringMap.find(key) != mStringMap.end())
	{
		return mStringMap[key];
	}

	return "";
}

int ConfigSystem::GetIntValue(std::string key)
{
	if (mIntMap.find(key) != mIntMap.end())
	{
		return mIntMap[key];
	}

	return 0;
}

double ConfigSystem::GetDoubleValue(std::string key)
{
	if (mDoubleMap.find(key) != mDoubleMap.end())
	{
		return mDoubleMap[key];
	}

	return 0;
}
