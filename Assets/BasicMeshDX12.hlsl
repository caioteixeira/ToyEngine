#pragma pack_matrix(row_major)

struct PointLightData {
	float4 diffuseColor;
	float4 specularColor;
	float3 position;
    float specularPower;
	float innerRadius;
	float outerRadius;
	bool enabled;
};

cbuffer PER_CAMERA : register(b0)
{
	float4x4 projMatrix;
	float3 worldCameraPos;

	float3 ambientLight;
	PointLightData pointLights[8];
}

cbuffer PER_OBJECT : register(b1)
{
	float4x4 worldMatrix;
}

cbuffer PER_MATERIAL : register(b2)
{
	float4 kd;
	float4 ks;
	float4 ka;
	float ns;
	float d;
	float tr;
}

SamplerState pointWrapSampler : register(s0);
SamplerState pointClampSampler : register(s1);
SamplerState linearWrapSampler : register(s2);
SamplerState linearClampSampler : register(s3);
SamplerState anisotropicWrapSampler : register(s4);
SamplerState anisotropicClampSampler : register(s5);
Texture2D DiffuseTexture : register(t0);

// Input structs for vertex and pixel shader
struct VS_INPUT
{
	float3 mPos : POSITION;
	float3 mNormal : NORMAL;
	float2 mTex : TEXCOORD;
};

struct PS_INPUT
{
	float4 mPos : SV_POSITION;
	float3 mNormal : NORMAL;
	float3 mWorldPosition : POSITION;
	float2 mTex: TEXCOORD;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output;

	float4  temp = float4(input.mPos, 1);
	temp = mul(temp, worldMatrix);
	output.mPos = mul(temp, projMatrix);
	output.mTex = input.mTex;

	float4 tempNormal = float4(input.mNormal, 0);
    tempNormal = mul(tempNormal, worldMatrix);
	output.mNormal = tempNormal.xyz;
	output.mWorldPosition = temp.xyz;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float4 color = DiffuseTexture.Sample(linearWrapSampler, input.mTex);

	//ambient
	float3 phong = ambientLight;

	//point
	float3 N = normalize(input.mNormal);
	float3 V = worldCameraPos - input.mWorldPosition;
	V = normalize(V);

	for (int i = 0; i < 8; i++)
	{
		if (!pointLights[i].enabled)
		{
			continue;
		}

		float3 L = pointLights[i].position - input.mWorldPosition;
		L = normalize(L);
		float NdotL = dot(N, L);

		float3 R = reflect(-L, N);

		if (NdotL > 0) {
			/*Diffuse color*/
			float dist = distance(input.mWorldPosition, pointLights[i].position);
			float sstep = smoothstep(pointLights[i].innerRadius, pointLights[i].outerRadius, dist);

			float4 diffuseColor = lerp(pointLights[i].diffuseColor, float4(0.0f, 0.0f, 0.0f, 0.0f), sstep);
			phong += (diffuseColor.xyz * NdotL);

			/*Specular color*/
			float RdotV = dot(R, V);
			phong += pointLights[i].specularColor.xyz * pow(max(0.0f, RdotV), pointLights[i].specularPower);
		}
	}

	//saturate phong
    float4 finalPhong = float4(phong, 0.0f);

    return color * saturate(finalPhong);
}
