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
		static CVar* GetCVar(std::string);
	private:

		static std::unordered_map<std::string, CVar> mCVarMap;
	};

	enum CVarType
	{
		String,
		Integer,
		Double
	};

	class CVar
	{
	public:
		static CVar* Get(std::string key);

		CVarType type;
		union
		{
			char stringValue[64];
			int intValue;
			double doubleValue;
		};
	};
}
