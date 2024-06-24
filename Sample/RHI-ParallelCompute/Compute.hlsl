#if VULKAN
#define VK_BINDING(x, y) [[vk::binding(x, y)]]
#else
#define VK_BINDING(x, y)
#endif

VK_BINDING(0, 0) cbuffer input : register(b0)
{
    float4 v[16];
};

VK_BINDING(1, 0) RWStructuredBuffer<float4> output : register(u0);

[numthreads(16, 1, 1)]
void CSMain(int id : SV_DispatchThreadID) {
    output[id.x] = v[id.x] * v[id.x];
}

