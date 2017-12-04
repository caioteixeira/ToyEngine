#include "D3D12Renderer.h"
#include "D3D12CommandContextManager.h"
#include "../ConstantBufferStructs.h"
#include "../FramePacket.h"
#include <easy/profiler.h>

D3D12Renderer::D3D12Renderer(): mWindow(nullptr), mThreadPool(NUM_THREADS - 1)
{
}


D3D12Renderer::~D3D12Renderer()
{
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
		SDL_Log("Could not create window.");
		return false;
	}

	mProj = Matrix::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(90.0f),
		static_cast<float>(mWidth) / static_cast<float>(mHeight), 0.5f, 500.0f);

	mGraphicsDevice = std::make_unique<D3D12GraphicsDevice>(GetActiveWindow());
	mResourceManager = std::make_unique<D3D12ResourceManager>(mGraphicsDevice.get());

	return true;
}

void D3D12Renderer::RenderFrame(FramePacket & framePacket)
{
	EASY_FUNCTION(profiler::colors::Blue);

	Clear();

	GlobalConstants constantBuffer;
	constantBuffer.projMatrix = framePacket.viewMatrix * mProj;

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
				
				PIXBeginEvent(context->GetCommandList(), 0, L"Rendering Job");

				auto globalCB = context->ReserveUploadMemory(sizeof GlobalConstants);
				memcpy(globalCB.CPUAddress, &constantBuffer, sizeof(GlobalConstants));

				auto element = pair.first;
				while(element < pair.second)
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

					auto& textureDescriptor = material->diffuseTexture->GetGraphicsTexture()->descriptor;
					CD3DX12_CPU_DESCRIPTOR_HANDLE CPUDescriptor(textureDescriptor->GetCPUDescriptorHandleForHeapStart());
					const auto GPUDescriptor = context->CopyDescriptorToDynamicHeap(CPUDescriptor);

					auto& materialCB = context->ReserveUploadMemory(sizeof(MaterialConstants));
					MaterialConstants materialConstants;
					materialConstants.kd = material->diffuseColor;
					materialConstants.ks = material->specularColor;
					materialConstants.ka = material->ambientColor;
					materialConstants.ns = material->shininess;

					memcpy(materialCB.CPUAddress, &materialConstants, sizeof(MaterialConstants));

					context->SetIndexBuffer(element->mesh->GetIndexBuffer());
					context->SetVertexBuffer(element->mesh->GetVertexBuffer());
					context->SetPrimitiveTopology(EPT_TriangleList);

					context->SetGraphicsRootDescriptorTable(0, GPUDescriptor);
					context->SetGraphicsRootConstantBufferView(1, globalCB.GPUAddress);
					context->SetGraphicsRootConstantBufferView(2, objectCB.GPUAddress);
					context->SetGraphicsRootConstantBufferView(3, materialCB.GPUAddress);

					context->DrawIndexed(element->mesh->indexCount, 0);

					EASY_END_BLOCK

					++element;
				}

				PIXEndEvent(context->GetCommandList());
				context->Finish();

				EASY_END_BLOCK
			});
		}
	});

	jobQueue.wait_job_actively(renderJob);

	EASY_END_BLOCK;

	Present();
}

D3D12ResourceManager * D3D12Renderer::GetResourceManager() const
{
	return mResourceManager.get();
}

void D3D12Renderer::Clear() const
{
	EASY_FUNCTION();
	mGraphicsDevice->ClearBackBuffer(Vector3(0.0f, 0.0f, 0.0f), 1.0f);
}

void D3D12Renderer::Present() const
{
	EASY_FUNCTION();
	mGraphicsDevice->Present();
}