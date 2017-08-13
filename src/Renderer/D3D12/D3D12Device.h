#pragma once
#ifdef DX12

#include <memory>

#include "../../WindowsHeaders.h"
#include "../../Math.h"
#include "D3D12CommandQueue.h"

class D3D12CommandQueue;

class D3D12GraphicsDevice
{
public:
	D3D12GraphicsDevice(void *window);
	~D3D12GraphicsDevice();

	void ClearBackBuffer(const Vector3& inColor, float inAlpha);

	void CreateCommandList(D3D12_COMMAND_LIST_TYPE type, ID3D12GraphicsCommandList** list, ID3D12CommandAllocator** allocator);

	void Present();

	ID3D12Device* GetDevice() const { return mDevice.Get(); }
private:
	void InitDevice();
	void InitCommandObjects();

	void FlushCommandQueue();

	ID3D12Resource* CurrentBackBuffer();
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView();
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView();

	void OnResize();
	void CreateSwapChain();

	void CreateRtvAndDsvDescriptorHeaps();

	HWND mWindow;
	UINT mWindowWidth;
	UINT mWindowHeight;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> mDevice;

	UINT64 mCurrentFence = 0;

	std::unique_ptr<D3D12CommandQueue> mGraphicsQueue;
	std::unique_ptr<D3D12CommandQueue> mCopyQueue;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	const DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
};

#endif
