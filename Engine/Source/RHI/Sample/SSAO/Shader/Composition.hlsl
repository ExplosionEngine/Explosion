#include <Platform.h>

VkBinding(0, 0) Texture2D posTex : register(t0);
VkBinding(1, 0) Texture2D normalTex : register(t1);
VkBinding(2, 0) Texture2D albedoTex : register(t2);
VkBinding(3, 0) Texture2D ssaoTex : register(t3);
VkBinding(4, 0) Texture2D ssaoBluredTex : register(t4);
VkBinding(5, 0) SamplerState texSampler : register(s0);
VkBinding(6, 0) cbuffer passParams : register(b0)
{
    float4x4 _dummy;
    int ssao;
    int ssaoOnly;
    int ssaoBlur;
}

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

float4 PSMain(VSOutput input) : SV_TARGET
{
	float3 fragposition = posTex.Sample(texSampler, input.uv).rgb;
	float3 normal = normalize(normalTex.Sample(texSampler, input.uv).rgb * 2.0 - 1.0);
	float4 albedo = albedoTex.Sample(texSampler, input.uv);

	float ssao = (ssaoBlur == 1) ? ssaoBluredTex.Sample(texSampler, input.uv).r : ssaoTex.Sample(texSampler, input.uv).r;

	float3 lightposition = float3(0.0, 0.0, 0.0);
	float3 L = normalize(lightposition - fragposition);
	float NdotL = max(0.5, dot(normal, L));

	float4 outFragColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	if (ssaoOnly == 1) {
		outFragColor.rgb = ssao.rrr;
	} else {
		float3 baseColor = albedo.rgb * NdotL;

		if (ssao == 1) {
			outFragColor.rgb = ssao.rrr;

			if (ssaoOnly != 1) {
			    outFragColor.rgb *= baseColor;
			}
		} else {
			outFragColor.rgb = baseColor;
		}
	}
	return outFragColor;
}