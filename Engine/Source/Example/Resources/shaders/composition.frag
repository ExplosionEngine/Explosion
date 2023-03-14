// Copyright 2020 Google LLC

Texture2D textureposition : register(t0);
Texture2D textureNormal : register(t1);
Texture2D textureAlbedo : register(t2);
Texture2D textureSSAO : register(t3);
Texture2D textureSSAOBlur : register(t4);

SamplerState texSampler : register(s0);

struct UBO
{
	float4x4 _dummy;
	int ssao;
	int ssaoOnly;
	int ssaoBlur;
};
cbuffer uboParams : register(b0) { UBO uboParams; };

float4 FSMain(float2 inUV : TEXCOORD0) : SV_TARGET
{
	float3 fragPos = textureposition.Sample(texSampler, inUV).rgb;
	float3 normal = normalize(textureNormal.Sample(texSampler, inUV).rgb * 2.0 - 1.0);
	float4 albedo = textureAlbedo.Sample(texSampler, inUV);

	float ssao = (uboParams.ssaoBlur == 1) ? textureSSAOBlur.Sample(texSampler, inUV).r : textureSSAO.Sample(texSampler, inUV).r;

	float3 lightPos = float3(0.0, 0.0, 0.0);
	float3 L = normalize(lightPos - fragPos);
	float NdotL = max(0.5, dot(normal, L));

	float4 outFragColor;
	if (uboParams.ssaoOnly == 1)
	{
		outFragColor.rgb = ssao.rrr;
	}
	else
	{
		float3 baseColor = albedo.rgb * NdotL;

		if (uboParams.ssao == 1)
		{
			outFragColor.rgb = ssao.rrr;

			if (uboParams.ssaoOnly != 1)
				outFragColor.rgb *= baseColor;
		}
		else
		{
			outFragColor.rgb = baseColor;
		}
	}
	return outFragColor;
}