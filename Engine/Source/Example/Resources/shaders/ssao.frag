// Copyright 2020 Google LLC

Texture2D texturePositionDepth : register(t0);
Texture2D textureNormal : register(t1);
Texture2D ssaoNoiseTexture : register(t2);

SamplerState texSampler : register(s0);
SamplerState ssaoNoiseSampler : register(s1);

#define SSAO_KERNEL_ARRAY_SIZE 64
const int SSAO_KERNEL_SIZE = 64;
const float SSAO_RADIUS = 0.5;

struct UBOSSAOKernel
{
	float4 samples[SSAO_KERNEL_ARRAY_SIZE];
};
cbuffer uboSSAOKernel : register(b0) { UBOSSAOKernel uboSSAOKernel; };

struct UBO
{
	float4x4 projection;
};
cbuffer ubo : register(b1) { UBO ubo; };

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
	for(int i = 0; i < SSAO_KERNEL_SIZE; i++)
	{
		float3 samplePos = mul(TBN, uboSSAOKernel.samples[i].xyz);
		samplePos = fragPos + samplePos * SSAO_RADIUS;

		// project
		float4 offset = float4(samplePos, 1.0f);
		offset = mul(ubo.projection, offset);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.5f;

		float sampleDepth = -texturePositionDepth.Sample(texSampler, offset.xy).w;

		float rangeCheck = smoothstep(0.0f, 1.0f, SSAO_RADIUS / abs(fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z ? 1.0f : 0.0f) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / float(SSAO_KERNEL_SIZE));

	return occlusion;
}

