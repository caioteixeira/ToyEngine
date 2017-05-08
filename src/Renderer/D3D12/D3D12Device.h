#pragma once
#ifdef DX12

#include "../../WindowsHeaders.h"
#include "../../Math.h"
#include "d3dx12.h"

class D3D12GraphicsDevice
{
public:
	D3D12GraphicsDevice(void *window);
	~D3D12GraphicsDevice();

	void ClearBackBuffer(const Vector3& inColor, float inAlpha);
	void Present() const;
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
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> mDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
	

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;
};

#endif
