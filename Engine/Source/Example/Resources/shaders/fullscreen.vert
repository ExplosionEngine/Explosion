// Copyright 2020 Google LLC

struct VSOutput
{
	float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD0;
};

VSOutput VSMain(float4 postion : POSITION0, float2 uv : TEXCOORD0)
{
	VSOutput output = (VSOutput)0;
	output.UV = uv;
	output.Pos = postion;
	return output;
}
