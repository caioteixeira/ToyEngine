#include "ConfigSystem.h"

using namespace Engine;

std::unordered_map<std::string, CVar> ConfigSystem::mCVarMap = {};


void ConfigSystem::Init()
{
	//TODO: Load configVars.json
}

CVar* ConfigSystem::GetCVar(std::string key)
{
	if(mCVarMap.find(key) != mCVarMap.end())
	{
		return &mCVarMap[key];
	}

	return nullptr;
}

CVar* CVar::Get(std::string key)
{
	return ConfigSystem::GetCVar(key);
}




