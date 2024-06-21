#if VULKAN
#define VK_BINDING(x, y) [[vk::binding(x, y)]]
#else
#define VK_BINDING(x, y)
#endif

struct Data {
    float3 v1;
    float2 v2;
}

VK_BINDING(0, 0) StructuredBuffer<Data> inputA : register(t0)
VK_BINDING(1, 0) StructuredBuffer<Data> inputB : register(t1)

VK_BINDING(2, 0) RWStructuredBuffer<Data> output : register(u0)

[numthreads(32, 1, 1)]
void CSMain(int id : SV_DispatchThreadID) {
    output[id.x].v1 = inputA[id.x].v1 + inputB[id.x].v1;
    output[id.x].v2 = inputA[id.x].v2 + inputB[id.x].v2;
}

