struct FragmentInput {
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0;
};

FragmentInput VSMain(float4 position : POSITION, float2 uv : TEXCOORD0)
{
    FragmentInput fragmentInput;
    fragmentInput.position = position;
    fragmentInput.uv = uv;
#if VULKAN
    fragmentInput.position.y = - fragmentInput.position.y;
    fragmentInput.uv.y = 1.0 - fragmentInput.uv.y;
#endif
    return fragmentInput;
}

Texture2D       textureColor : register(t0);
SamplerState    samplerColor : register(s0);

float4 FSMain(FragmentInput input) : SV_TARGET
{
    return textureColor.Sample(samplerColor, input.uv);
}
