#if VULKAN
#define VK_BINDING(x, y) [[vk::binding(x, y)]]
#define VK_LOCATION(x)   [[vk::location(x)]]
#else
#define VK_BINDING(x, y)
#define VK_LOCATION(x)
#endif

VK_BINDING(0, 0) Texture2D       textureColor : register(t0);
VK_BINDING(1, 0) SamplerState    samplerColor : register(s0);

VK_BINDING(2, 0) cbuffer UBO : register(b0)
{
	float4x4 model;
};

struct FragmentInput {
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

FragmentInput VSMain(
    VK_LOCATION(0) float4 position : POSITION,
    VK_LOCATION(1) float2 uv : TEXCOORD)
{
    FragmentInput fragmentInput;
    fragmentInput.position = mul(model, position);
    fragmentInput.uv = uv;
#if VULKAN
    fragmentInput.uv.y = 1 - fragmentInput.uv.y;
#endif
    return fragmentInput;
}

float4 PSMain(FragmentInput input) : SV_TARGET
{
    return textureColor.Sample(samplerColor, input.uv);
}
