//
// Created by johnk on 9/1/2022.
//

#include <vector>

#include <glm/glm.hpp>

#include <Application.h>
#include <RHI/Gpu.h>
#include <RHI/Device.h>
#include <RHI/SwapChain.h>
#include <RHI/Queue.h>
#include <RHI/Buffer.h>
#include <RHI/BufferView.h>
using namespace RHI;

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

class TriangleApplication : public Application {
public:
    NON_COPYABLE(TriangleApplication)
    explicit TriangleApplication(const std::string& n) : Application(n) {}
    ~TriangleApplication() override = default;

protected:
    void OnCreate() override
    {
        instance = Instance::CreateByType(rhiType);
        gpu = instance->GetGpu(0);

        std::vector<QueueInfo> queueCreateInfos = {{ QueueType::GRAPHICS, 1 }};
        DeviceCreateInfo deviceCreateInfo {};
        deviceCreateInfo.queueCreateInfoNum = queueCreateInfos.size();
        deviceCreateInfo.queueCreateInfos = queueCreateInfos.data();
        device = gpu->RequestDevice(&deviceCreateInfo);

        SwapChainCreateInfo swapChainCreateInfo {};
        swapChainCreateInfo.format = PixelFormat::RGBA8_UNORM;
        swapChainCreateInfo.presentMode = PresentMode::IMMEDIATELY;
        swapChainCreateInfo.textureNum = 2;
        swapChainCreateInfo.extent = { width, height };
        swapChainCreateInfo.window = GetPlatformWindow();
        swapChainCreateInfo.presentQueue = graphicsQueue;
        // TODO create swap chain

        graphicsQueue = device->GetQueue(QueueType::GRAPHICS, 0);

        std::vector<Vertex> vertices = {
            { { -.5f, -.5f, 0.f }, { 1.f, 0.f, 0.f } },
            { { .5f, -.5f, 0.f }, { 0.f, 1.f, 0.f } },
            { { 0.f, .5f, 0.f }, { 0.f, 0.f, 1.f } },
        };
        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferCreateInfo.usages = BufferUsageBits::VERTEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        vertexBuffer = device->CreateBuffer(&bufferCreateInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
        }

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.vertex.stride = sizeof(Vertex);
        vertexBufferView = vertexBuffer->CreateBufferView(&bufferViewCreateInfo);
    }

private:
    Instance* instance = nullptr;
    Gpu* gpu = nullptr;
    Device* device = nullptr;
    Queue* graphicsQueue = nullptr;
    SwapChain* swapChain = nullptr;
    Buffer* vertexBuffer = nullptr;
    BufferView* vertexBufferView = nullptr;
    RHIType rhiType = RHIType::DIRECTX_12;
};

int main(int argc, char* argv[])
{
    TriangleApplication application("RHI-Triangle");
    return application.Run(argc, argv);
}
