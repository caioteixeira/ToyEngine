#pragma once
#ifdef DX12

#include <memory>

#include "../../WindowsHeaders.h"
#include "../../Math.h"
#include "D3D12CommandQueue.h"
#include "D3D12Types.h"
#include "d3dx12.h"


class D3D12CommandQueue;
class CommandContextManager;
class D3D12CommandContext;

class D3D12GraphicsDevice
{

public:

	D3D12GraphicsDevice(void *window);
	~D3D12GraphicsDevice();

	Microsoft::WRL::ComPtr<ID3DBlob> CompileShaderFromFile(const std::wstring& filename, const D3D_SHADER_MACRO* defines, 
		const std::string& entrypoint, const std::string& target);
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(CD3DX12_ROOT_SIGNATURE_DESC& desc);
	GraphicsBufferPtr CreateGraphicsBuffer(const std::string& name, UINT numElements, UINT elementSize, const void* initialData);
	GraphicsTexturePtr CreateTextureFromFile(const char* inFileName, int& outWidth, int& outHeight) const;

	void ClearBackBuffer(const Vector3& inColor, float inAlpha);

	void Present();

	ID3D12Device* GetDevice() const { return mDevice.Get(); }
	CommandContextManager* GetCommandContextManager() const { return mCommandListManager.get(); }
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

	std::unique_ptr<CommandContextManager> mCommandListManager;

	HWND mWindow;
	UINT mWindowWidth;
	UINT mWindowHeight;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> mDevice;

	UINT64 mCurrentFence = 0;

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
