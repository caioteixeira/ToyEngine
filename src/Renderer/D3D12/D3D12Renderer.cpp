#include "D3D12Renderer.h"
#include "D3D12CommandContextManager.h"
#include "../ConstantBufferStructs.h"
#include "../FramePacket.h"
#include <easy/profiler.h>
#include "../../WindowsHeaders.h"
#include <SDL_syswm.h>
#include "imgui.h"
#include "imgui_impl_dx12.h"

using namespace Engine;

D3D12Renderer::D3D12Renderer(): mWindow(nullptr), mThreadPool(NUM_THREADS - 1)
{
}


D3D12Renderer::~D3D12Renderer()
{
    ImGui_ImplDX12_Shutdown();
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

    mGraphicsDevice = std::make_unique<D3D12GraphicsDevice>(GetActiveWindow());
    mResourceManager = std::make_unique<D3D12ResourceManager>(mGraphicsDevice.get());

    InitImgui();

    return true;
}

void D3D12Renderer::InitImgui()
{
    mImguiDescriptorHeap = mGraphicsDevice->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                                                 D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1);

    struct SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    SDL_GetWindowWMInfo(mWindow, &wmInfo);

    ImGui::CreateContext();
    ImGui_ImplDX12_Init(wmInfo.info.win.window, mGraphicsDevice->SwapChainBufferCount, mGraphicsDevice->GetDevice(),
                        DXGI_FORMAT_R8G8B8A8_UNORM, mImguiDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                        mImguiDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

    SetupImguiNewFrame();
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

    //Divide work and create parallel tasks
    auto chunks = DivideWork(framePacket.meshes.begin(), framePacket.meshes.end(), NUM_THREADS);
    auto& jobQueue = mThreadPool.GetQueue();
    auto renderJob = jobQueue.create_job([this, &chunks, &constantBuffer](jobxx::context& ctx)
    {
        for (auto& pair : chunks)
        {
            auto contextManager = mGraphicsDevice->GetCommandContextManager();
            auto context = contextManager->AllocateContext();
            context->SetScissor(mGraphicsDevice->GetScissorRect());
            context->SetRenderTarget(mGraphicsDevice->CurrentBackBufferView(), mGraphicsDevice->DepthStencilView());
            context->SetViewport(mGraphicsDevice->GetViewPort());

            ctx.spawn_task([context, &pair, &constantBuffer]()
            {
                EASY_BLOCK("Rendering Job", profiler::colors::Green);

                auto globalCB = context->ReserveUploadMemory(sizeof GlobalConstants);
                memcpy(globalCB.CPUAddress, &constantBuffer, sizeof(GlobalConstants));

                auto element = pair.first;
                while (element < pair.second)
                {
                    EASY_BLOCK("RenderElement");

                    PerObjectConstants objectConstants;
                    objectConstants.worldTransform = element->worldTransform;

                    auto& objectCB = context->ReserveUploadMemory(sizeof(PerObjectConstants));
                    memcpy(objectCB.CPUAddress, &objectConstants, sizeof(PerObjectConstants));

                    auto& material = element->material;
                    auto& pipelineState = material->pipelineState;
                    context->SetPipelineState(pipelineState);
                    context->SetGraphicsRootSignature(pipelineState->rootSignature.Get());
                    context->SetDynamicDescriptorHeap();

                    if (material->properties & DiffuseTexture)
                    {
                        auto& textureDescriptor = material->diffuseTexture->GetGraphicsTexture()->descriptor;
                        CD3DX12_CPU_DESCRIPTOR_HANDLE
                            CPUDescriptor(textureDescriptor->GetCPUDescriptorHandleForHeapStart());
                        const auto GPUDescriptor = context->CopyDescriptorToDynamicHeap(CPUDescriptor);
                        context->SetGraphicsRootDescriptorTable(0, GPUDescriptor);
                    }

                    auto& materialCB = context->ReserveUploadMemory(sizeof(MaterialConstants));
                    MaterialConstants materialConstants;
                    materialConstants.kd = material->diffuseColor.ToVector3();
                    materialConstants.ks = material->specularColor.ToVector3();
                    materialConstants.ka = material->ambientColor.ToVector3();
                    materialConstants.ns = material->shininess;

                    memcpy(materialCB.CPUAddress, &materialConstants, sizeof(MaterialConstants));

                    context->SetIndexBuffer(element->mesh->GetIndexBuffer());
                    context->SetVertexBuffer(element->mesh->GetVertexBuffer());
                    context->SetPrimitiveTopology(EPT_TriangleList);

                    context->SetGraphicsRootConstantBufferView(1, globalCB.GPUAddress);
                    context->SetGraphicsRootConstantBufferView(2, objectCB.GPUAddress);
                    context->SetGraphicsRootConstantBufferView(3, materialCB.GPUAddress);

                    context->DrawIndexed(element->mesh->indexCount, 0);

                    EASY_END_BLOCK

                    ++element;
                }

                context->Finish();

                EASY_END_BLOCK
            });
        }
    });

    jobQueue.wait_job_actively(renderJob);

    //Render ImGuiFrame
    mImguiContext->SetScissor(mGraphicsDevice->GetScissorRect());
    mImguiContext->SetRenderTarget(mGraphicsDevice->CurrentBackBufferView(), mGraphicsDevice->DepthStencilView());
    mImguiContext->SetViewport(mGraphicsDevice->GetViewPort());
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData());

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
    auto contextManager = mGraphicsDevice->GetCommandContextManager();
    mImguiContext = contextManager->AllocateContext();
    mImguiContext->SetDescriptorHeap(mImguiDescriptorHeap);
    ImGui_ImplDX12_NewFrame(mImguiContext->GetCommandList());
}

void D3D12Renderer::Present()
{
    EASY_FUNCTION();
    mGraphicsDevice->Present();

    SetupImguiNewFrame();
}
