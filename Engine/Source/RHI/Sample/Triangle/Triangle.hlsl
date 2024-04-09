#if VULKAN
#define VK_LOCATION(x) [[vk::location(x)]]
#else
#define VK_LOCATION(x)
#endif

struct FragmentInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

FragmentInput VSMain(
    VK_LOCATION(0) float4 position : POSITION,
    VK_LOCATION(1) float4 color : COLOR)
{
    FragmentInput fragmentInput;
    fragmentInput.position = position;
#if VULKAN
    fragmentInput.position.y = - fragmentInput.position.y;
#endif
    fragmentInput.color = color;
    return fragmentInput;
}

float4 FSMain(FragmentInput input) : SV_TARGET
{
    return input.color;
}