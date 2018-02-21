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

	for (auto itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
	{
		if(itr->value.IsString())
		{
			CVar var;
			var.type = CVarType::String;
			strcpy(var.stringValue, itr->value.GetString());
			mCVarMap.emplace(itr->name.GetString(), var);
		}
		else if(itr->value.IsInt())
		{
			CVar var;
			var.type = CVarType::Integer;
			var.intValue = itr->value.GetInt();
			mCVarMap.emplace(itr->name.GetString(), var);
		}
		else if(itr->value.IsDouble())
		{
			CVar var;
			var.type = CVarType::Double;
			var.doubleValue = itr->value.GetDouble();
			mCVarMap.emplace(itr->name.GetString(), var);
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




