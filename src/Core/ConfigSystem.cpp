#include "ConfigSystem.h"
#include <iostream>
#include <fstream>
#include <document.h>
#include "Logger.h"
#include "imgui/imgui.h"

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
		else if(itr->value.IsFloat())
		{
			CVar var;
			var.type = CVarType::Float;
			var.floatValue = itr->value.GetFloat();
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

	Logger::DebugLogError("No CVar for key %c", key.c_str());
	return nullptr;
}

void ConfigSystem::DrawDebugWindow()
{
	ImGui::Begin("CVars");

	for (auto pair : mCVarMap)
	{
		ImGui::PushID(pair.first.c_str());
		
		auto var = &pair.second; 
		switch(var->type)
		{
			case String:
			{
				ImGui::InputText(pair.first.c_str(), var->stringValue, 64);
				break;
			}
			case Integer:
			{
				ImGui::InputInt(pair.first.c_str(), &var->intValue);
				break;
			}
			case Float:
			{
				ImGui::InputFloat(pair.first.c_str(), &var->floatValue);
				break;
			}
		}

		ImGui::PopID();
	}
	ImGui::End();
}

CVar* CVar::Get(std::string key)
{
	return ConfigSystem::GetCVar(key);
}




