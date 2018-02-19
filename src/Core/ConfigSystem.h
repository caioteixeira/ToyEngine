#pragma once
#include <string>
#include <unordered_map>

namespace Engine
{
	class ConfigSystem
	{
		friend class Game;
		friend class CVar;
	public:
		static void Init();
	private:
		static std::string GetStringValue(std::string);
		static int GetIntValue(std::string);
		static double GetDoubleValue(std::string);

		static std::unordered_map<std::string, std::string> mStringMap;
		static std::unordered_map<std::string, int> mIntMap;
		static std::unordered_map<std::string, double> mDoubleMap;
	};

	class CVar
	{
	public:
		static std::string GetString(std::string key)
		{
			return ConfigSystem::GetStringValue(key);
		}

		static int GetInt(std::string key)
		{
			return ConfigSystem::GetIntValue(key);
		}

		static double GetDouble(std::string key)
		{
			return ConfigSystem::GetDoubleValue(key);
		}
	};
}
