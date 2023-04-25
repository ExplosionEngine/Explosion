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
