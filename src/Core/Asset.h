#pragma once
#include <memory>

namespace Engine
{
    class Asset : public std::enable_shared_from_this<Asset>
    {
    public:
        Asset(class Game& game);
        virtual ~Asset();

    protected:
        using enable_shared_from_this<Asset>::shared_from_this;
        virtual bool Load(const char* fileName, class AssetCache* cache) = 0;
        class Game& mGame;
    };

    typedef std::shared_ptr<Asset> AssetPtr;
}

//TODO: Move to a macro definition file
// Use this as DECL_ASSET(SelfClass, SuperClass)
#define DECL_ASSET(d,s) typedef s Super; \
	std::shared_ptr<d> ThisPtr() \
	{ \
		return std::static_pointer_cast<d>(shared_from_this()); \
	} \
	public: \
	static std::shared_ptr<d> StaticLoad(const char* file, class AssetCache* cache, Game& game) \
	{ \
		std::shared_ptr<d> ptr = std::make_shared<d>(game); \
		if (!ptr->Load(file, cache)) { return nullptr; } \
		return ptr; \
	} \
	private:
