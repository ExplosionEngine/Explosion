//
// Created by johnk on 9/1/2022.
//

#include <vector>

#include <glm/glm.hpp>

#include <Application.h>
#include <RHI/Gpu.h>
#include <RHI/Device.h>
#include <RHI/Queue.h>
#include <RHI/Buffer.h>
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

        graphicsQueue = device->GetQueue(QueueType::GRAPHICS, 0);

        std::vector<Vertex> vertices = {
            { { -.5f, -.5f, 0.f }, { 1.f, 0.f, 0.f } },
            { { .5f, -.5f, 0.f }, { 0.f, 1.f, 0.f } },
            { { 0.f, .5f, 0.f }, { 0.f, 0.f, 1.f } },
        };

        BufferCreateInfo createInfo {};
        createInfo.size = vertices.size() * sizeof(Vertex);
        createInfo.usages = BufferUsageBits::VERTEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        vertexBuffer = device->CreateBuffer(&createInfo);
        if (vertexBuffer != nullptr) {
            auto* data = vertexBuffer->Map(MapMode::WRITE, 0, createInfo.size);
            memcpy(data, vertices.data(), createInfo.size);
        }
    }

private:
    Instance* instance {};
    Gpu* gpu {};
    Device* device {};
    Queue* graphicsQueue {};
    Buffer* vertexBuffer {};
    RHIType rhiType = RHIType::DIRECTX_12;
};

int main(int argc, char* argv[])
{
    TriangleApplication application("RHI-Triangle");
    return application.Run(argc, argv);
}
