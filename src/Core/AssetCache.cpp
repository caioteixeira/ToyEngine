#include "AssetCache.h"

using namespace Engine;

AssetCache::AssetCache(Game& game, const char* rootDirectory): 
	mGame(game), mRoot(rootDirectory)
{
}

AssetCache::~AssetCache()
{
}

void AssetCache::Clear()
{
	mAssetMap.clear();
}
