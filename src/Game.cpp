#include "Game.h"
#include "ECS/quick.h"
#include "GameWorld.h"
#include <chrono>
#include <easy/profiler.h>
#include "EngineCore.h"
#include "Graphics/Renderer.h"

using namespace Engine;

Game::Game():
    mWorld()
    , mInput(*this)
    , mShouldQuit(false)
{
    mRenderer = std::make_shared<Renderer>();

    ConfigSystem::Init();
}

Game::~Game()
{
}

bool Game::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
    {
        Logger::DebugLog("Failed to initialize SDL.");
        return false;
    }

    const auto windowWidth = CVar::Get("windowWidth")->intValue;
    const auto windowHeight = CVar::Get("windowHeight")->intValue;
    if (!mRenderer->Init(windowWidth, windowHeight))
    {
        Logger::DebugLog("Failed to initialized Renderer.");
    }

    StartGame();

    return true;
}

void Game::RunLoop()
{
    EASY_PROFILER_ENABLE
    EASY_MAIN_THREAD;
    profiler::startListen();

    auto lastFrame = std::chrono::high_resolution_clock::now();
    while (!mShouldQuit)
    {
        EASY_BLOCK("GameLoop")

        auto now = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(now - lastFrame).count() /
            1000.0f;
        lastFrame = now;

        mInput.ProcessInput();

        ConfigSystem::DrawDebugWindow();

        //TODO: Run systems
        mWorld.Update(delta);

        EASY_END_BLOCK;
    }

    profiler::dumpBlocksToFile("test_profile.prof");
}

void Game::StartGame()
{
    mWorld.Init(mRenderer);

    const auto scene = CVar::Get("initialScene");
    assert(scene != nullptr);
    mWorld.LoadScene(scene->stringValue);
}
