#ifdef DX11
#include "D3D11Renderer.h"
#include "D3D11ResourceManager.h"

D3D11Renderer::D3D11Renderer()
	: mWindow(nullptr)
{
	
}

D3D11Renderer::~D3D11Renderer()
{
}

bool D3D11Renderer::Init(int width, int height)
{
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

	return true;
}

void D3D11Renderer::InitFrameBuffer()
{
	auto rasterizerState = mGraphicsDevice->CreateRasterizerState(EFM_Solid);
	mGraphicsDevice->SetRasterizerState(rasterizerState);

	mDepthBuffer = mGraphicsDevice->CreateDepthStencil(mWidth, mHeight);
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

void D3D11Renderer::RenderFrame(FramePacket& packet)
{
	Clear();
	//TODO: Draw
	Present();
}

void D3D11Renderer::Clear() const
{
	mGraphicsDevice->ClearBackBuffer(Vector3::Zero, 1.0f);
}

void D3D11Renderer::Present() const
{
	mGraphicsDevice->Present();
}
#endif

