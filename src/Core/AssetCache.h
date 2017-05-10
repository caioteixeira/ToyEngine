#pragma once
#include <unordered_map>
#include "Asset.h"

namespace Engine
{
	class Game;

	class AssetCache
	{
	public:
		AssetCache(Game& game, const char* rootDirectory);
		~AssetCache();

		template<typename T>
		std::shared_ptr<T> Load(const char * fileName)
		{
			std::string path(mRoot);
			path += fileName;
			auto iter = mAssetMap.find(path);

			if(iter != mAssetMap.end())
			{
				return std::static_pointer_cast<T>(iter->second);
			}

			std::shared_ptr<T> asset = T::StaticLoad(path.c_str(), this, mGame);
			if(asset)
			{
				mAssetMap.emplace(path, asset);
			}

			return asset;
		}

		void Clear();
	private:
		std::unordered_map<std::string, AssetPtr> mAssetMap;
		Game& mGame;
		const char* mRoot;
	};
}

