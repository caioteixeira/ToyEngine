#include <easy/profiler.h>
#ifdef DX11
#include "D3D11Renderer.h"
#include "D3D11ResourceManager.h"
#include "../ConstantBufferStructs.h"
#include "../FramePacket.h"
#include "../../Core/imgui/imgui.h"
#include "../../Core/imgui/imgui_impl_dx11.h"

D3D11Renderer::D3D11Renderer()
	: mWindow(nullptr)
{
	
}

D3D11Renderer::~D3D11Renderer()
{
	delete[] mWindowName;
}

bool D3D11Renderer::Init(int width, int height)
{
	mWindowName = new char[200];
	mWindow = SDL_CreateWindow("Toy Engine", 100, 100, width, height, 0);
	mWidth = width;
	mHeight = height;

	if(!mWindow)
	{
		SDL_Log("Could not create window.");
		return false;
	}

	mGraphicsDevice = std::make_unique<D3D11GraphicsDevice>(GetActiveWindow());
	mResourceManager = std::make_unique<D3D11ResourceManager>(*mGraphicsDevice);

	InitFrameBuffer();

	InitShaders();

	mProj = Matrix::CreatePerspectiveFieldOfView(DirectX::XMConvertToRadians(90.0f),
	                                             static_cast<float>(mWidth) / static_cast<float>(mHeight), 0.5f, 500.0f);
	mCameraBuffer = mGraphicsDevice->CreateGraphicsBuffer(nullptr, sizeof(GlobalConstants), EBF_ConstantBuffer, ECPUAF_CanWrite, EGBU_Dynamic);

	//Setup ImGUI
	ImGui_ImplDX11_Init(GetActiveWindow(), mGraphicsDevice->GetDevice(), mGraphicsDevice->GetImmediateContext());

	return true;
}

void D3D11Renderer::InitFrameBuffer()
{
	auto rasterizerState = mGraphicsDevice->CreateRasterizerState(EFM_Solid);
	mGraphicsDevice->SetRasterizerState(rasterizerState);

	mDepthBuffer = mGraphicsDevice->CreateDepthStencil();
	mGraphicsDevice->SetDepthStencil(mDepthBuffer);

	mMeshDepthState = mGraphicsDevice->CreateDepthStencilState(true, EComparisonFunc::ECF_Less);
	mMeshBlendState = mGraphicsDevice->CreateBlendState(false);
}

void D3D11Renderer::InitShaders()
{
	std::vector<char> temp;
	//Compile and load pixel shader
	mGraphicsDevice->CompileShaderFromFile("Assets/BasicMesh.hlsl", "PS", "ps_4_0", temp);
	mPixelShader = mGraphicsDevice->CreatePixelShader(temp);
	temp.clear();

	//Compile and load vertex shader
	mGraphicsDevice->CompileShaderFromFile("Assets/BasicMesh.hlsl", "VS", "vs_4_0", temp);
	mVertexShader = mGraphicsDevice->CreateVertexShader(temp);

	InputLayoutElement vs_mesh_elements[] = {
		InputLayoutElement("POSITION", 0, EGF_R32G32B32_Float, 0),
		InputLayoutElement("NORMAL", 0, EGF_R32G32B32_Float, 12),
		InputLayoutElement("TEXCOORD", 0, EGF_R32G32_Float, 24),
	};
	auto vs_mesh_layout = mGraphicsDevice->CreateInputLayout(vs_mesh_elements, 3, temp);
	mResourceManager->RegisterInputLayout("positionnormaltexcoord", vs_mesh_layout);

	//Create sampler
	mDefaultSampler = mGraphicsDevice->CreateSamplerState();
	mGraphicsDevice->SetPSSamplerState(mDefaultSampler, 0);
}

void D3D11Renderer::DrawMeshElement(MeshElement& element) const
{
	PerObjectConstants objectConstants;
	objectConstants.worldTransform = element.worldTransform;
	void* buffer = mGraphicsDevice->MapBuffer(element.constantBuffer);
	memcpy(buffer, &objectConstants, sizeof(PerObjectConstants));
	mGraphicsDevice->UnmapBuffer(element.constantBuffer);

	mGraphicsDevice->SetVSConstantBuffer(element.constantBuffer, 1);

	mGraphicsDevice->SetInputLayout(element.material->pipelineState->inputLayout);
	mGraphicsDevice->SetVertexBuffer(element.mesh->GetVertexBuffer(), sizeof(Vertex));
	mGraphicsDevice->SetIndexBuffer(element.mesh->GetIndexBuffer());
	mGraphicsDevice->SetPSTexture(element.material->diffuseTexture->GetGraphicsTexture(), 0);
	mGraphicsDevice->DrawIndexed(element.mesh->indexCount, 0, 0);
}

void D3D11Renderer::UpdateGlobalConstants(FramePacket& packet) const
{
	GlobalConstants constants;
	constants.projMatrix = packet.viewMatrix *  mProj;
	void* buffer = mGraphicsDevice->MapBuffer(mCameraBuffer);
	memcpy(buffer, &constants, sizeof(GlobalConstants));
	mGraphicsDevice->UnmapBuffer(mCameraBuffer);
}

void D3D11Renderer::RenderFrame(FramePacket& packet)
{
	snprintf(mWindowName, 200, "ToyEngine : %llu FPS", profiler::main_thread::frameTimeLocalAvg());
	SDL_SetWindowTitle(mWindow, mWindowName);

	EASY_FUNCTION();

	Clear();

	mGraphicsDevice->SetDepthStencilState(mMeshDepthState);
	mGraphicsDevice->SetBlendState(mMeshBlendState);

	mGraphicsDevice->SetVertexShader(mVertexShader);
	mGraphicsDevice->SetPixelShader(mPixelShader);
	mGraphicsDevice->SetPSSamplerState(mDefaultSampler, 0);

	UpdateGlobalConstants(packet);

	mGraphicsDevice->SetVSConstantBuffer(mCameraBuffer, 0);
	mGraphicsDevice->SetPSConstantBuffer(mCameraBuffer, 0);

	for(MeshElement& element : packet.meshes)
	{
		DrawMeshElement(element);
	}

	//ImGui::Render();

	Present();
}

void D3D11Renderer::Clear() const
{
	mGraphicsDevice->ClearBackBuffer(Vector3::Zero, 1.0f);
	mGraphicsDevice->ClearDepthStencil(mDepthBuffer, 1.0f);
}

void D3D11Renderer::Present() const
{
	mGraphicsDevice->Present();
}
#endif

