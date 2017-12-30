#pragma pack_matrix(row_major)

struct PointLightData 
{
	float3 diffuseColor;
    float innerRadius;
    float3 position;
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
	float3 kd;
    float d;
	float3 ks;
    float ns;
	float3 ka;
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

//
// Lighting Utils
//  (from Frank D.Luna DX12 Samples)
//
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff.
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

// Schlick gives an approximation to Fresnel reflectance (see pg. 233 "Real-Time Rendering 3rd Ed.").
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

    return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye)
{
    const float m = ns * 256.0f;
    float3 halfVec = normalize(toEye + lightVec);

    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;

    float3 fresnelFactor = SchlickFresnel(ks, halfVec, lightVec);

    float3 specAlbedo = fresnelFactor * roughnessFactor;

    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (kd.rgb + specAlbedo) * lightStrength;
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;

    float4 temp = float4(input.mPos, 1);
    temp = mul(temp, worldMatrix);
    output.mPos = mul(temp, projMatrix);
    output.mTex = input.mTex;
    output.mNormal = mul(input.mNormal, (float3x3) worldMatrix);
    output.mWorldPosition = temp.xyz;

    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
    float4 color = DiffuseTexture.Sample(anisotropicWrapSampler, input.mTex);

	//ambient light
    float3 phong = ambientLight;

	// Point light
    float3 N = normalize(input.mNormal);
    float3 V = worldCameraPos - input.mWorldPosition;
    V = normalize(V);

    for (int i = 0; i < 8; i++)
    {
        PointLightData lightData = pointLights[i];
        if (!lightData.enabled)
        {
            continue;
        }

        // The vector from the surface to the light.
        float3 lightVec = lightData.position - input.mWorldPosition;

        // The distance from surface to light.
        float d = length(lightVec);

        // Range test.
        if (d > lightData.outerRadius)
            continue;

        // Normalize the light vector.
        lightVec /= d;

        // Scale light down by Lambert's cosine law.
        float ndotl = max(dot(lightVec, N), 0.0f);
        float3 lightStrength = lightData.diffuseColor * ndotl;

        // Attenuate light by distance.
        float att = CalcAttenuation(d, lightData.innerRadius, lightData.outerRadius);
        lightStrength *= att;
        
        phong += BlinnPhong(lightStrength, lightVec, N, V);
    }

	//saturate phong
    float4 finalPhong = float4(phong, 0.0f);

    return color * saturate(finalPhong);
}
