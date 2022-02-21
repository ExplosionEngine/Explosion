//
// Created by johnk on 9/1/2022.
//

#include <vector>

#include <glm/glm.hpp>

#include <Application.h>
#include <RHI/Instance.h>
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
    TriangleApplication(const std::string& n, const uint32_t w, const uint32_t h) : Application(n, w, h) {}
    ~TriangleApplication() override = default;

protected:
    void OnCreate() override
    {
        instance = Instance::CreateByType(rhiType);
        gpu = instance->GetGpu(0);

        {
            std::vector<QueueInfo> queueCreateInfos = {
                { QueueType::GRAPHICS, 2 },
                { QueueType::COMPUTE, 1 }
            };
            DeviceCreateInfo deviceCreateInfo {};
            deviceCreateInfo.queueCreateInfoNum = queueCreateInfos.size();
            deviceCreateInfo.queueCreateInfos = queueCreateInfos.data();
            device = gpu->RequestDevice(&deviceCreateInfo);
        }

        {
            graphicsQueue = device->GetQueue(QueueType::COMPUTE, 0);
        }

        {
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
    }

    void OnStart(int argc, char* argv[]) override
    {
        if (argc < 2) {
            return;
        }
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--rhi") {
                if (i + 1 < argc) {
                    std::string rhi = argv[++i];
                    if (rhi == std::string("vulkan")) {
                        rhiType = RHIType::VULKAN;
                    } else if (rhi == std::string("dx12")) {
                        rhiType = RHIType::DIRECTX_12;
                    }
                }
            }
        }
    }

    void OnDestroy() override
    {
        if (device != nullptr) {
            delete device;
            device = nullptr;
        }
    }

    void OnDrawFrame() override {}

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
    TriangleApplication application("RHI-Triangle", 1024, 768);
    application.Start(argc, argv);
    return application.Run();
}
