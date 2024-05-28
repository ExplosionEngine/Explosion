#include "Common.h"

VK_BINDING(0, 0) cbuffer UBO : register(b0)
{
	float4x4 projection;
	float4x4 model;
	float4x4 view;
	float nearPlane;
	float farPlane;
};

VK_BINDING(0, 1) Texture2D textureColorMap : register(t0, space1);
VK_BINDING(1, 1) SamplerState samplerColorMap : register(s0, space1);

struct VSInput
{
    VK_LOCATION(0) float4 Pos : POSITION;
    VK_LOCATION(1) float2 UV : TEXCOORD;
    VK_LOCATION(2) float3 Color : COLOR;
    VK_LOCATION(3) float3 Normal : NORMAL;
};

struct VSOutput
{
	float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 Color : COLOR;
    float3 WorldPos : POSITION;
};

VSOutput VSMain(VSInput input)
{
	VSOutput output = (VSOutput)0;
	output.Pos = mul(projection, mul(view, mul(model, input.Pos)));

	output.UV = input.UV;

	// Vertex position in view space
	output.WorldPos = mul(view, mul(model, input.Pos)).xyz;

	// Normal in view space
	float3x3 normalMatrix = (float3x3)mul(model, view);
	output.Normal = mul(input.Normal, normalMatrix);

#if VULKAN
    output.Pos.y = -output.Pos.y;
    output.Normal.y = -output.Normal.y;
#endif

	output.Color = input.Color;
	return output;
}

struct FSOutput
{
	float4 Position : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Albedo : SV_TARGET2;
};

float linearDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;
	return (2.0f * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));
}

FSOutput PSMain(VSOutput input)
{
	FSOutput output = (FSOutput)0;
	output.Position = float4(input.WorldPos, linearDepth(input.Pos.z));
	output.Normal = float4(normalize(input.Normal) * 0.5 + 0.5, 1.0);
	output.Albedo = textureColorMap.Sample(samplerColorMap, input.UV) * float4(input.Color, 1.0);
	return output;
}