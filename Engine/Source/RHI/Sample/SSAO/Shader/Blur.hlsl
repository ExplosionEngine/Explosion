#include "Common.h"

VK_BINDING(0, 0) Texture2D textureSSAO : register(t0);
VK_BINDING(1, 0) SamplerState samplerSSAO : register(s0);

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

float4 FSMain(VSOutput input) : SV_TARGET
{
	const int blurRange = 2;
	int n = 0;
	int2 texDim;
	textureSSAO.GetDimensions(texDim.x, texDim.y);
	float2 texelSize = 1.0 / (float2)texDim;
	float result = 0.0;
	for (int x = -blurRange; x < blurRange; x++)
	{
		for (int y = -blurRange; y < blurRange; y++)
		{
			float2 offset = float2(float(x), float(y)) * texelSize;
			result += textureSSAO.Sample(samplerSSAO, input.UV + offset).r;
			n++;
		}
	}
	return result / (float(n));
}