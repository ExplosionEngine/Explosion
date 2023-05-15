#include "Common.h"

struct VSOutput
{
	float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 UV : TEXCOORD;
    float3 Color : COLOR;
    float3 WorldPos : POSITION;
};

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

FSOutput FSMain(VSOutput input)
{
	FSOutput output = (FSOutput)0;
	output.Position = float4(input.WorldPos, linearDepth(input.Pos.z));
	output.Normal = float4(normalize(input.Normal) * 0.5 + 0.5, 1.0);
	output.Albedo = textureColorMap.Sample(samplerColorMap, input.UV) * float4(input.Color, 1.0);
	return output;
}