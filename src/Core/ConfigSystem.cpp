#include "ConfigSystem.h"
#include <iostream>
#include <fstream>
#include <document.h>
#include "Logger.h"
#include <array>
#include "imgui.h"

using namespace Engine;

std::unordered_map<std::string, int> ConfigSystem::mCVarMap = {};
std::array<CVar, ConfigSystem::CVAR_REGISTRY_SIZE> ConfigSystem::mCVars = {};
int ConfigSystem::mLastCVarIndex = 0;

void ConfigSystem::Init()
{
    memset(mCVars.data(), 0, sizeof(CVar) * mCVars.size());

    //TODO: Load config.json
    std::ifstream configFile("config.json");
    if (!configFile.is_open())
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
    if (document.Parse(json.c_str()).HasParseError())
    {
        Logger::DebugLogError("Failed to parse config.json!");
    }

    for (auto itr = document.MemberBegin(); itr != document.MemberEnd(); ++itr)
    {
        CVar var;
        if (itr->value.IsString())
        {
            var.type = String;
            strcpy(var.stringValue, itr->value.GetString());
        }
        else if (itr->value.IsInt())
        {
            var.type = Integer;
            var.intValue = itr->value.GetInt();
        }
        else if (itr->value.IsFloat())
        {
            var.type = Float;
            var.floatValue = itr->value.GetFloat();
        }

        mCVars[mLastCVarIndex] = var;
        mCVarMap.emplace(itr->name.GetString(), mLastCVarIndex);
        mLastCVarIndex++;
    }
}

CVar* ConfigSystem::GetCVar(std::string key)
{
    if (mCVarMap.find(key) != mCVarMap.end())
    {
        auto index = mCVarMap[key];
        return &mCVars[index];
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

        const auto index = pair.second;
        auto var = &mCVars[index];
        switch (var->type)
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
