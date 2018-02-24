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
		static void DrawDebugWindow();
	private:

		static int mLastCVarIndex;
		static constexpr int CVAR_REGISTRY_SIZE = 256;
		static std::array<CVar, CVAR_REGISTRY_SIZE> mCVars;
		static std::unordered_map<std::string, int> mCVarMap;
	};

	enum CVarType
	{
		String,
		Integer,
		Float
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
			float floatValue;
		};
	};
}
