#pragma pack_matrix(row_major)

cbuffer PER_CAMERA : register(b0)
{
	float4x4 projMatrix;
}

cbuffer PER_OBJECT : register(b1)
{
	float4x4 worldMatrix;
}

SamplerState DefaultSampler : register(s0);
Texture2D DiffuseTexture : register(t0);

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	float3 mPos : POSITION;
	float3 mNormal : NORMAL;
	float2 mTex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 mPos : SV_POSITION;
	float2 mTex: TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	// (For now, just directly copy position/tex coord)
	float4  temp = float4(input.mPos, 1);
	temp = mul(temp, worldMatrix);
	output.mPos = mul(temp, projMatrix);
	output.mTex = input.mTex;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	return DiffuseTexture.Sample(DefaultSampler, input.mTex);
}
