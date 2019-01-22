#include "D3D12Device.h"
#include "D3D12CommandContext.h"
#include "D3D12CommandContextManager.h"
#include "DynamicUploadHeap.h"

D3D12CommandContext::D3D12CommandContext(D3D12_COMMAND_LIST_TYPE type, D3D12GraphicsDevice* device)
    : mDevice(device), mUploadHeap(true, device, 4092)
{
    mContextManager = mDevice->GetCommandContextManager();
    mContextManager->CreateCommandList(type, &mCommandList, &mAllocator);
    mDynamicDescriptorHeap = std::make_unique<DynamicDescriptorHeap>(mDevice, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                                                     1024);
}

D3D12CommandContext::~D3D12CommandContext()
{
}

uint64_t D3D12CommandContext::Finish(bool waitCompletion)
{
    assert(mAllocator != nullptr);

    auto& queue = mContextManager->GetGraphicsQueue();

    const uint64_t fenceValue = queue.ExecuteCommandList(mCommandList);
    queue.StoreAllocator(fenceValue, mAllocator);
    mAllocator = nullptr;

    const uint64_t lastCompletedFence = queue.GetLastCompletedFenceValue();
    mUploadHeap.FinishFrame(fenceValue, lastCompletedFence);
    mDynamicDescriptorHeap->FinishCurrentFrame(fenceValue);
    mDynamicDescriptorHeap->ReleaseCompletedFrames(lastCompletedFence);

    if (waitCompletion)
    {
        mContextManager->WaitForFence(fenceValue);
    }

    mContextManager->FreeContext(this);

    return fenceValue;
}

void D3D12CommandContext::CopyBuffer(GraphicsResource* dest, GraphicsResource* src)
{
    TransitionResource(dest, D3D12_RESOURCE_STATE_COPY_DEST);
    TransitionResource(src, D3D12_RESOURCE_STATE_COPY_SOURCE);
    mCommandList->CopyResource(dest->buffer.Get(), src->buffer.Get());
}

DynamicAllocation D3D12CommandContext::ReserveUploadMemory(size_t sizeInBytes)
{
    return mUploadHeap.Allocate(sizeInBytes, 256);
}

void D3D12CommandContext::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle, Color& clearColor)
{
    float color[4];
    color[0] = clearColor.x;
    color[1] = clearColor.y;
    color[2] = clearColor.z;
    color[3] = 1.0f;

    mCommandList->ClearRenderTargetView(rtvHandle, color, 0, nullptr);
}

void D3D12CommandContext::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle, float depth, UINT8 stencil)
{
    mCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, depth, stencil, 0,
                                        nullptr);
}

CD3DX12_GPU_DESCRIPTOR_HANDLE D3D12CommandContext::CopyDescriptorToDynamicHeap(
    CD3DX12_CPU_DESCRIPTOR_HANDLE descriptor) const
{
    const auto alloc = mDynamicDescriptorHeap->AllocateDescriptor();
    mDevice->GetDevice()->CopyDescriptorsSimple(1, alloc.CPUHandle, descriptor, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    return alloc.GPUHandle;
}

void D3D12CommandContext::SetRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE& rtv, D3D12_CPU_DESCRIPTOR_HANDLE& dsv) const
{
    mCommandList->OMSetRenderTargets(1, &rtv, true, &dsv);
}

void D3D12CommandContext::SetGraphicsRootSignature(ID3D12RootSignature* rootSignature) const
{
    mCommandList->SetGraphicsRootSignature(rootSignature);
}

void D3D12CommandContext::SetGraphicsRootDescriptorTable(int index, D3D12_GPU_DESCRIPTOR_HANDLE descriptorTable) const
{
    mCommandList->SetGraphicsRootDescriptorTable(index, descriptorTable);
}

void D3D12CommandContext::SetPipelineState(PipelineState* state)
{
    if (mActualPipelineState == state)
    {
        return;
    }

    mCommandList->SetPipelineState(state->pipelineState.Get());
    mActualPipelineState = state;
}

void D3D12CommandContext::SetDescriptorHeap(ID3D12DescriptorHeap* heap)
{
    if (mActualDescriptorHeap == heap)
    {
        return;
    }
    mActualDescriptorHeap = heap;

    ID3D12DescriptorHeap* descriptorHeaps[] = {heap};
    mCommandList->SetDescriptorHeaps(1, descriptorHeaps);
}

void D3D12CommandContext::SetDynamicDescriptorHeap()
{
    SetDescriptorHeap(mDynamicDescriptorHeap->GetDescriptorHeap());
}

void D3D12CommandContext::SetDescriptorHeap(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap)
{
    SetDescriptorHeap(heap.Get());
}

void D3D12CommandContext::SetIndexBuffer(const GraphicsBufferPtr buffer)
{
    if (buffer->resource.state != D3D12_RESOURCE_STATE_INDEX_BUFFER)
    {
        TransitionResource(&buffer->resource, D3D12_RESOURCE_STATE_INDEX_BUFFER);
    }
    auto view = buffer->GetIndexBufferView();
    mCommandList->IASetIndexBuffer(&view);
}

void D3D12CommandContext::SetVertexBuffer(const GraphicsBufferPtr buffer)
{
    if (buffer->resource.state != D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)
    {
        TransitionResource(&buffer->resource, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    }
    auto view = buffer->GetVertexBufferView();
    mCommandList->IASetVertexBuffers(0, 1, &view);
}

void D3D12CommandContext::SetPrimitiveTopology(EPrimitiveTopology topology)
{
    mCommandList->IASetPrimitiveTopology(static_cast<D3D12_PRIMITIVE_TOPOLOGY>(topology));
}

void D3D12CommandContext::Draw(int vertexCount, int vertexStartOffset)
{
    mCommandList->DrawInstanced(vertexCount, 1, vertexStartOffset, 0);
}

void D3D12CommandContext::DrawIndexed(int indexCount, int startIndexLocation, int baseVertexLocation)
{
    mCommandList->DrawIndexedInstanced(indexCount, 1, startIndexLocation, baseVertexLocation, 0);
}

void D3D12CommandContext::SetGraphicsRootConstantBufferView(int slot, D3D12_GPU_VIRTUAL_ADDRESS location)
{
    mCommandList->SetGraphicsRootConstantBufferView((UINT)slot, location);
}

void D3D12CommandContext::TransitionResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES stateBefore,
                                             D3D12_RESOURCE_STATES stateAfter)
{
    assert(stateBefore != stateAfter);

    D3D12_RESOURCE_BARRIER barrierDesc;

    barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrierDesc.Transition.pResource = resource;
    barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrierDesc.Transition.StateBefore = stateBefore;
    barrierDesc.Transition.StateAfter = stateAfter;

    //TODO: Verify if a transition already started
    barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

    //TODO: Cache multiple barriers and delay submission
    mCommandList->ResourceBarrier(1, &barrierDesc);
}

void D3D12CommandContext::TransitionResource(GraphicsResource* resource, D3D12_RESOURCE_STATES state)
{
    TransitionResource(resource->buffer.Get(), resource->state, state);
    resource->state = state;
}

void D3D12CommandContext::Reset()
{
    assert(mCommandList != nullptr && mAllocator == nullptr);
    mAllocator = mContextManager->GetGraphicsQueue().GetAllocator();
    mCommandList->Reset(mAllocator, nullptr);

    mActualPipelineState = nullptr;
    mActualDescriptorHeap = nullptr;
}

void D3D12CommandContext::SetViewport(const D3D12_VIEWPORT& viewport)
{
    mCommandList->RSSetViewports(1, &viewport);
}

void D3D12CommandContext::SetScissor(const D3D12_RECT& rect)
{
    assert(rect.left < rect.right && rect.top < rect.bottom);
    mCommandList->RSSetScissorRects(1, &rect);
}
