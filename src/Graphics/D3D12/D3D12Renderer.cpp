#include "D3D12Renderer.h"
#include "D3D12CommandContextManager.h"
#include "../ConstantBufferStructs.h"
#include "../FramePacket.h"
#include <easy/profiler.h>
#include "../../WindowsHeaders.h"
#include <SDL_syswm.h>
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx12.h"

using namespace Engine;

D3D12Renderer::D3D12Renderer(): mWindow(nullptr)
{
    SDL_Log("Created Renderer!");
}


D3D12Renderer::~D3D12Renderer()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    delete[] mWindowName;
}

bool D3D12Renderer::Init(int width, int height)
{
    mWindowName = new char[200];
    mWindow = SDL_CreateWindow("Toy Engine", 100, 100, width, height, 0);
    mWidth = width;
    mHeight = height;

    if (!mWindow)
    {
        Logger::DebugLog("Could not create window.");
        return false;
    }

    struct SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    SDL_GetWindowWMInfo(mWindow, &wmInfo);
    const auto hwnd = wmInfo.info.win.window;

    mGraphicsDevice = std::make_unique<D3D12GraphicsDevice>(hwnd);
    mResourceManager = std::make_unique<D3D12ResourceManager>(mGraphicsDevice.get());

    InitImgui(hwnd);

    return true;
}

void D3D12Renderer::InitImgui(const HWND hwnd)
{
    mImguiDescriptorHeap = mGraphicsDevice->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                                                 D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(mGraphicsDevice->GetDevice(), mGraphicsDevice->SwapChainBufferCount,
                        DXGI_FORMAT_R8G8B8A8_UNORM, mImguiDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                        mImguiDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    ImGui::StyleColorsDark();

    SetupImguiNewFrame();
}

void inline D3D12Renderer::RenderMesh(D3D12CommandContext* context, const DynamicAllocation globalCB, std::vector<MeshElement>::value_type element) const
{
    PerObjectConstants objectConstants;
    objectConstants.worldTransform = element.worldTransform;

    auto& objectCB = context->ReserveUploadMemory(sizeof(PerObjectConstants));
    memcpy(objectCB.CPUAddress, &objectConstants, sizeof(PerObjectConstants));

    auto& material = element.material;
    const auto pipelineState = mResourceManager->GetPipelineState(material->pipelineState);
    context->SetPipelineState(pipelineState);
    context->SetGraphicsRootSignature(pipelineState->rootSignature.Get());
    context->SetDynamicDescriptorHeap();

    if (material->properties & DiffuseTexture)
    {
        auto diffuseTexture = mResourceManager->GetTexture(material->diffuseTexture);
        auto& textureDescriptor = diffuseTexture->descriptor;
        CD3DX12_CPU_DESCRIPTOR_HANDLE
            CPUDescriptor(textureDescriptor->GetCPUDescriptorHandleForHeapStart());
        const auto GPUDescriptor = context->CopyDescriptorToDynamicHeap(CPUDescriptor);
        context->SetGraphicsRootDescriptorTable(0, GPUDescriptor);
    }

    const auto geometry = element.geometry;
    const auto indexBuffer = mResourceManager->GetGraphicsBuffer(geometry.IndexBuffer);
    const auto vertexBuffer = mResourceManager->GetGraphicsBuffer(geometry.VertexBuffer);
    const auto materialBuffer = mResourceManager->GetGraphicsBuffer(material->constantBuffer);

    context->SetIndexBuffer(indexBuffer);
    context->SetVertexBuffer(vertexBuffer);
    context->SetPrimitiveTopology(EPT_TriangleList);

    context->SetGraphicsRootConstantBufferView(1, globalCB.GPUAddress);
    context->SetGraphicsRootConstantBufferView(2, objectCB.GPUAddress);
    context->SetGraphicsRootConstantBufferView(3, materialBuffer->resource.buffer->GetGPUVirtualAddress());

    context->DrawIndexed(indexBuffer->numElements, 0);
}

void D3D12Renderer::RenderFrame(FramePacket& framePacket)
{
    EASY_FUNCTION(profiler::colors::Blue);

    Clear();

    GlobalConstants constantBuffer;
    memset(&constantBuffer, 0, sizeof GlobalConstants);
    constantBuffer.viewProjMatrix = framePacket.viewProjMatrix;
    constantBuffer.cameraPos = framePacket.cameraPos;
    constantBuffer.ambientColor = framePacket.ambientLightColor;
    memcpy(&constantBuffer.pointLights, &framePacket.pointLights, sizeof PointLightData * MAX_POINT_LIGHTS);

    EASY_BLOCK("Render Elements", profiler::colors::BlueGrey);

    auto contextManager = mGraphicsDevice->GetCommandContextManager();
    auto context = contextManager->AllocateContext();
    context->SetScissor(mGraphicsDevice->GetScissorRect());
    context->SetRenderTarget(mGraphicsDevice->CurrentBackBufferView(), mGraphicsDevice->DepthStencilView());
    context->SetViewport(mGraphicsDevice->GetViewPort());

    const auto globalCB = context->ReserveUploadMemory(sizeof GlobalConstants);
    memcpy(globalCB.CPUAddress, &constantBuffer, sizeof(GlobalConstants));
    for (auto element : framePacket.meshes)
    {
        EASY_BLOCK("RenderElement");
        RenderMesh(context, globalCB, element);
        EASY_END_BLOCK
    }

    context->Finish();

    //Render ImGuiFrame
    mImguiContext = contextManager->AllocateContext();
    mImguiContext->SetRenderTarget(mGraphicsDevice->CurrentBackBufferView(), mGraphicsDevice->DepthStencilView());
    mImguiContext->SetScissor(mGraphicsDevice->GetScissorRect());
    mImguiContext->SetViewport(mGraphicsDevice->GetViewPort());
    mImguiContext->SetDescriptorHeap(mImguiDescriptorHeap);

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mImguiContext->GetCommandList());

    mImguiContext->Finish();

    EASY_END_BLOCK;

    Present();
}

D3D12ResourceManager* D3D12Renderer::GetResourceManager() const
{
    return mResourceManager.get();
}

void D3D12Renderer::Clear() const
{
    EASY_FUNCTION();
    mGraphicsDevice->ClearBackBuffer(Vector3(0.0f, 0.0f, 0.0f), 1.0f);
}

void D3D12Renderer::SetupImguiNewFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void D3D12Renderer::Present()
{
    EASY_FUNCTION();
    mGraphicsDevice->Present();

    SetupImguiNewFrame();
}
