//
// Created by johnk on 9/1/2022.
//

#include <vector>

#include <Application.h>
#include <RHI/Instance.h>
#include <RHI/Gpu.h>
#include <RHI/Device.h>
#include <RHI/Queue.h>
using namespace RHI;

class TriangleApplication : public Application {
public:
    NON_COPYABLE(TriangleApplication)
    TriangleApplication(const std::string& n, const uint32_t w, const uint32_t h) : Application(n, w, h) {}
    ~TriangleApplication() override = default;

protected:
    void OnCreate() override
    {
        instance = Instance::CreateByType(RHIType::VULKAN);
        gpu = instance->GetGpu(0);

        {
            std::vector<QueueCreateInfo> queueCreateInfos = {
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
    }

    void OnDestroy() override {}

    void OnDrawFrame() override {}

private:
    Instance* instance = nullptr;
    Gpu* gpu = nullptr;
    Device* device = nullptr;
    Queue* graphicsQueue = nullptr;
};

int main(int argc, char* argv[])
{
    TriangleApplication application("RHI-Triangle", 1024, 768);
    return application.Run();
}
