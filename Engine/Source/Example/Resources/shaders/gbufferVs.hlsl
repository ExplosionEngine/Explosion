#include "common.h"

struct VSInput
{
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;
    float3 Color : COLOR;
    float3 Normal : NORMAL;
};

VK_BINDING(0, 0) cbuffer UBO : register(b0)
{
	float4x4 projection;
	float4x4 model;
	float4x4 view;
	float nearPlane;
	float farPlane;
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
	output.Pos = mul(mul(mul(input.Pos, model), view), projection);

	output.UV = input.UV;

	// Vertex position in view space
	output.WorldPos = mul(mul(input.Pos, model), view).xyz;

	// Normal in view space
	float3x3 normalMatrix = (float3x3)mul(view, model);
	output.Normal = mul(normalMatrix, input.Normal);

	output.Color = input.Color;
	return output;
}
