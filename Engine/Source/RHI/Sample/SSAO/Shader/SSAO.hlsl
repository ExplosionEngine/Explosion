#include <Platform.h>

VkBinding(0, 0) Texture2D posDepthTex : register(t0);
VkBinding(1, 0) Texture2D normalTex : register(t1);
VkBinding(2, 0) Texture2D ssaoNoiseTex : register(t2);
VkBinding(3, 0) SamplerState texSampler : register(s0);
VkBinding(4, 0) SamplerState ssaoNoiseSampler : register(s1);
VkBinding(5, 0) cbuffer kernalParams : register(b0)
{
	float4 randomKernals[64];
};
VkBinding(6, 0) cbuffer passParams : register(b1)
{
	float4x4 projection;
};

struct VSOutput
{
	float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(
    VkLocation(0) float4 postion : POSITION,
    VkLocation(1) float2 uv : TEXCOORD)
{
	VSOutput output = (VSOutput)0;
	output.uv = uv;
	output.position = postion;

#if VULKAN
    output.uv.y = 1 - output.uv.y;
#endif

	return output;
}

float PSMain(VSOutput input) : SV_TARGET
{
	// Get G-Buffer values
	float3 fragposition = posDepthTex.Sample(texSampler, input.uv).rgb;
	float3 normal = normalize(normalTex.Sample(texSampler, input.uv).rgb * 2.0 - 1.0);

	// Get a random vector using a noise lookup
	int2 texDim;
	posDepthTex.GetDimensions(texDim.x, texDim.y);
	int2 noiseDim;
	ssaoNoiseTex.GetDimensions(noiseDim.x, noiseDim.y);
	const float2 noiseuv = float2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/(noiseDim.y)) * input.uv;
	float3 randomVec = ssaoNoiseTex.Sample(ssaoNoiseSampler, noiseuv).xyz * 2.0 - 1.0;

	// Create TBN matrix
	float3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	float3 bitangent = cross(tangent, normal);
	float3x3 TBN = transpose(float3x3(tangent, bitangent, normal));

	// Calculate occlusion value
	float occlusion = 0.0f;
	for(int i = 0; i < 64; i++)
	{
		float3 sampleposition = mul(TBN, randomKernals[i].xyz);
		sampleposition = fragposition + sampleposition * 0.2; // ssao radius is 0.2

		// project
		float4 offset = float4(sampleposition, 1.0f);
		offset = mul(projection, offset);
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5f + 0.2f;

		float sampleDepth = -posDepthTex.Sample(texSampler, offset.xy).w;

		float rangeCheck = smoothstep(0.0f, 1.0f, 0.5 / abs(fragposition.z - sampleDepth));
		occlusion += (sampleDepth >= sampleposition.z ? 1.0f : 0.0f) * rangeCheck;
	}
	occlusion = 1.0 - (occlusion / 64.0);
	return occlusion;
}