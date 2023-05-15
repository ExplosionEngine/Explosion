#include "Common.h"

VK_BINDING(0, 0) Texture2D texturePositionDepth : register(t0);
VK_BINDING(1, 0) Texture2D textureNormal : register(t1);
VK_BINDING(2, 0) Texture2D ssaoNoiseTexture : register(t2);

VK_BINDING(3, 0) SamplerState texSampler : register(s0);
VK_BINDING(4, 0) SamplerState ssaoNoiseSampler : register(s1);

VK_BINDING(5, 0) cbuffer RandomKernals : register(b0)
{
	float4 randomKernals[64];
};

VK_BINDING(6, 0) cbuffer UBO : register(b1)
{
	float4x4 projection;
};

float FSMain(float2 inUV : TEXCOORD0) : SV_TARGET
{
	// Get G-Buffer values
	float3 fragPos = texturePositionDepth.Sample(texSampler, inUV).rgb;
	float3 normal = normalize(textureNormal.Sample(texSampler, inUV).rgb * 2.0 - 1.0);

	// Get a random vector using a noise lookup
	int2 texDim;
	texturePositionDepth.GetDimensions(texDim.x, texDim.y);
	int2 noiseDim;
	ssaoNoiseTexture.GetDimensions(noiseDim.x, noiseDim.y);
	const float2 noiseUV = float2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/(noiseDim.y)) * inUV;
	float3 randomVec = ssaoNoiseTexture.Sample(ssaoNoiseSampler, noiseUV).xyz * 2.0 - 1.0;

	// Create TBN matrix
	float3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	float3 bitangent = cross(tangent, normal);
	float3x3 TBN = transpose(float3x3(tangent, bitangent, normal));

	// Calculate occlusion value
	float occlusion = 0.0f;
	for(int i = 0; i < 64; i++)
	{
		float3 samplePos = mul(randomKernals[i].xyz, TBN);
		samplePos = fragPos + samplePos * 0.1; // ssao radius is 0.1

		// project
		float4 offset = float4(samplePos, 1.0f);
		offset = mul(offset, projection);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float sampleDepth = -texturePositionDepth.Sample(texSampler, offset.xy).w;

		float rangeCheck = smoothstep(0.0f, 1.0f, 0.1 / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z ? 1.0f : 0.0f) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / 64.0);

	return occlusion;
}

