#pragma once
#include "DynamicDescriptorHeap.h"

#include "../../Math.h"
#include "D3D12Types.h"
#include "DynamicUploadHeap.h"

class D3D12CommandQueue;
class D3D12GraphicsDevice;
class CommandContextManager;

class D3D12CommandContext
{
    friend D3D12CommandQueue;

public:
    explicit D3D12CommandContext(D3D12_COMMAND_LIST_TYPE type, D3D12GraphicsDevice* device);
    ~D3D12CommandContext();

    //NonCopyable
    D3D12CommandContext() = default;
    D3D12CommandContext(const D3D12CommandContext&) = delete;
    D3D12CommandContext& operator=(const D3D12CommandContext&) = delete;

    uint64_t Flush(bool waitCompletion = false);
    uint64_t Finish(bool waitCompletion = false);

    void CopyBuffer(GraphicsResource* dest, GraphicsResource* src);
    DynamicAllocation ReserveUploadMemory(size_t sizeInBytes);

    void ClearRenderTargetView(const D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle, Color& clearColor);
    void ClearDepthStencilView(const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle, float depth, UINT8 stencil);

    CD3DX12_GPU_DESCRIPTOR_HANDLE CopyDescriptorToDynamicHeap(CD3DX12_CPU_DESCRIPTOR_HANDLE descriptor) const;

    void SetRenderTarget(const D3D12_CPU_DESCRIPTOR_HANDLE& rtv, const D3D12_CPU_DESCRIPTOR_HANDLE& dsv) const;

    void SetGraphicsRootSignature(ID3D12RootSignature* rootSignature) const;
    void SetGraphicsRootDescriptorTable(int index, D3D12_GPU_DESCRIPTOR_HANDLE descriptorTable) const;
    void SetPipelineState(PipelineState* state);
    void SetDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap);
    void SetDescriptorHeap(ID3D12DescriptorHeap* heap);
    void SetDynamicDescriptorHeap();

    void SetIndexBuffer(GraphicsBuffer* buffer);
    void SetVertexBuffer(GraphicsBuffer* buffer);
    void SetPrimitiveTopology(EPrimitiveTopology topology);

    void SetViewport(const D3D12_VIEWPORT& viewport);
    void SetScissor(const D3D12_RECT& rect);

    void Draw(int vertexCount, int vertexStartOffset = 0);
    void DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation = 0);

    void SetGraphicsRootConstantBufferView(int slot, D3D12_GPU_VIRTUAL_ADDRESS location);

    void TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore,
                            D3D12_RESOURCE_STATES stateAfter);
    void TransitionResource(GraphicsResource* resource, D3D12_RESOURCE_STATES state);

    ID3D12GraphicsCommandList* GetCommandList() { return mCommandList; }

    void Reset();
private:
    ID3D12GraphicsCommandList* mCommandList;
    ID3D12CommandAllocator* mAllocator;

    CommandContextManager* mContextManager;
    D3D12GraphicsDevice* mDevice;

    PipelineState* mActualPipelineState;
    ID3D12DescriptorHeap* mActualDescriptorHeap;

    DynamicUploadHeap mUploadHeap;
    std::unique_ptr<DynamicDescriptorHeap> mDynamicDescriptorHeap;
};
