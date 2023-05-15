#include "Common.h"

VK_BINDING(0, 0) Texture2D texturePosition : register(t0);
VK_BINDING(1, 0) Texture2D textureNormal : register(t1);
VK_BINDING(2, 0) Texture2D textureAlbedo : register(t2);
VK_BINDING(3, 0) Texture2D textureSSAO : register(t3);
VK_BINDING(4, 0) Texture2D textureSSAOBlur : register(t4);

VK_BINDING(5, 0) SamplerState texSampler : register(s0);

VK_BINDING(6, 0) cbuffer uboParams : register(b0)
{
    float4x4 _dummy;
    int ssao;
    int ssaoOnly;
    int ssaoBlur;
}

struct VSOutput
{
	float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;
};

VSOutput VSMain(float4 postion : POSITION, float2 uv : TEXCOORD)
{
	VSOutput output = (VSOutput)0;
	output.UV = uv;
	output.Pos = postion;

	return output;
}

float4 FSMain(float2 inUV : TEXCOORD) : SV_TARGET
{
	float3 fragPos = texturePosition.Sample(texSampler, inUV).rgb;
	float3 normal = normalize(textureNormal.Sample(texSampler, inUV).rgb * 2.0 - 1.0);
	float4 albedo = textureAlbedo.Sample(texSampler, inUV);

	float ssao = (ssaoBlur == 1) ? textureSSAOBlur.Sample(texSampler, inUV).r : textureSSAO.Sample(texSampler, inUV).r;

	float3 lightPos = float3(0.0, 0.0, 0.0);
	float3 L = normalize(lightPos - fragPos);
	float NdotL = max(0.5, dot(normal, L));

	float4 outFragColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if (ssaoOnly == 1)
	{
		outFragColor.rgb = ssao.rrr;
	}
	else
	{
		float3 baseColor = albedo.rgb * NdotL;

		if (ssao == 1)
		{
			outFragColor.rgb = ssao.rrr;

			if (ssaoOnly != 1)
				outFragColor.rgb *= baseColor;
		}
		else
		{
			outFragColor.rgb = baseColor;
		}
	}
	return outFragColor;
}