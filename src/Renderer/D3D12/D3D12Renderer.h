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
	void RenderFrame(FramePacket & framePacket);
	D3D12ResourceManager* GetResourceManager() const;
	void SetupImguiNewFrame();
private:
	void Clear() const;
	void Present();

	void InitImgui();

	std::unique_ptr<class D3D12GraphicsDevice> mGraphicsDevice;
	std::unique_ptr<D3D12ResourceManager> mResourceManager;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mImguiDescriptorHeap;
	D3D12CommandContext* mImguiContext;

	char * mWindowName;
	SDL_Window* mWindow;
	int mWidth = 0;
	int mHeight = 0;

	Matrix mProj;

	Engine::Core::WorkerPool mThreadPool;

	/**
	* \brief Divides a vector into N ranges
	* \tparam Iterator type
	* \param begin begin iterator
	* \param end end iterator
	* \param n number of chuncks
	* \return returns an array of ranges
	*/
	template <typename Iterator>
	std::vector<std::pair<Iterator, Iterator>> DivideWork(Iterator begin, Iterator end, size_t n)
	{
		std::vector<std::pair<Iterator, Iterator>> ranges;
		if (n == 0)
		{
			return ranges;
		}
		ranges.reserve(n);

		auto dist = std::distance(begin, end);
		n = std::min<size_t>(n, dist);
		auto chunk = dist / n;
		auto remainder = dist % n;

		for (size_t i = 0; i < n - 1; ++i)
		{
			auto next_end = std::next(begin, chunk + (remainder ? 1 : 0));
			ranges.emplace_back(begin, next_end);

			begin = next_end;
			if (remainder) remainder -= 1;
		}

		// last chunk
		ranges.emplace_back(begin, end);
		return ranges;
	}
};
