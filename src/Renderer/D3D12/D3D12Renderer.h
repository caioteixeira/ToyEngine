#pragma once
#include "../ConstantBufferStructs.h"

#include "../IRenderer.h"
#include "../../WindowsHeaders.h"
#include <memory>
#include "D3D12Device.h"
#include "D3D12ResourceManager.h"
#include "../../EngineCore.h"
#include "../FramePacket.h"

class D3D12Renderer : IRenderer
{
public:
    D3D12Renderer();
    ~D3D12Renderer();

    bool Init(int width, int height);
    void RenderFrame(FramePacket& framePacket);
    D3D12ResourceManager* GetResourceManager() const;
    void SetupImguiNewFrame();
private:
    void Clear() const;
    void Present();

    void InitImgui();
    void inline RenderMesh(D3D12CommandContext* context, DynamicAllocation globalCB,
                    std::vector<MeshElement>::value_type element) const;

    std::unique_ptr<class D3D12GraphicsDevice> mGraphicsDevice;
    std::unique_ptr<D3D12ResourceManager> mResourceManager;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mImguiDescriptorHeap;
    D3D12CommandContext* mImguiContext;

    char* mWindowName;
    SDL_Window* mWindow;
    int mWidth = 0;
    int mHeight = 0;

    Engine::Core::WorkerPool mThreadPool;
};
