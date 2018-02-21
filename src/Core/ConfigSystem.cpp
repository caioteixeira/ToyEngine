#include "ConfigSystem.h"
#include <iostream>
#include <fstream>
#include <document.h>
#include "Logger.h"

using namespace Engine;

std::unordered_map<std::string, CVar> ConfigSystem::mCVarMap = {};

void ConfigSystem::Init()
{
	//TODO: Load config.json
	std::ifstream configFile("config.json");
	if(!configFile.is_open())
	{
		Logger::DebugLogError("Failed to open config.json!");
		return;
	}

	std::string json;
	configFile.seekg(0, std::ios::end);
	json.reserve(configFile.tellg());
	configFile.seekg(0, std::ios::beg);
	json.assign((std::istreambuf_iterator<char>(configFile)), std::istreambuf_iterator<char>());

	rapidjson::Document document;
	if(document.Parse(json.c_str()).HasParseError())
	{
		Logger::DebugLogError("Failed to parse config.json!");
	}

	for (auto itr = document.Begin(); itr != document.End(); ++itr)
	{
		if(itr->IsString())
		{
			CVar var;
			var.type = CVarType::String;
			strcpy(var.stringValue, itr->GetString());
			mCVarMap.emplace("string", var);
		}
		else if(itr->IsInt())
		{
			CVar var;
			var.type = CVarType::Integer;
			var.intValue = itr->GetInt();
			mCVarMap.emplace("int", var);
		}
		else if(itr->IsDouble())
		{
			CVar var;
			var.type = CVarType::Double;
			var.doubleValue = itr->GetDouble();
			mCVarMap.emplace("double", var);
		}
	}
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




