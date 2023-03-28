struct FragmentInput {
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

FragmentInput VSMain(float4 position : POSITION, float2 uv : TEXCOORD)
{
    FragmentInput fragmentInput;
    fragmentInput.position = position;
    fragmentInput.uv = uv;
#if VULKAN
    fragmentInput.position.y = - fragmentInput.position.y;
#endif
    return fragmentInput;
}

[[vk::binding(0)]] Texture2D       textureColor : register(t0);
[[vk::binding(1)]] SamplerState    samplerColor : register(s0);

float4 FSMain(FragmentInput input) : SV_TARGET
{
    return textureColor.Sample(samplerColor, input.uv);
}
