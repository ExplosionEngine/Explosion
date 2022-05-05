//
// Created by johnk on 9/1/2022.
//

#include <vector>
#include <array>

#include <glm/glm.hpp>

#include <Application.h>
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
        CreateInstanceAndSelectGPU();
        RequestDeviceAndFetchQueues();
        CreateSwapChain();
        CreateVertexBuffer();
        CreatePipelineLayout();
        CreatePipeline();
    }

private:
    void CreateInstanceAndSelectGPU()
    {
        instance = Instance::CreateByType(rhiType);

        gpu = instance->GetGpu(0);
    }

    void RequestDeviceAndFetchQueues()
    {
        std::vector<QueueInfo> queueCreateInfos = {{ QueueType::GRAPHICS, 1 }};
        DeviceCreateInfo createInfo {};
        createInfo.queueCreateInfoNum = queueCreateInfos.size();
        createInfo.queueCreateInfos = queueCreateInfos.data();
        device = gpu->RequestDevice(&createInfo);
        graphicsQueue = device->GetQueue(QueueType::GRAPHICS, 0);
    }

    void CreateSwapChain()
    {
        SwapChainCreateInfo swapChainCreateInfo {};
        swapChainCreateInfo.format = PixelFormat::RGBA8_UNORM;
        swapChainCreateInfo.presentMode = PresentMode::IMMEDIATELY;
        swapChainCreateInfo.textureNum = 2;
        swapChainCreateInfo.extent = { width, height };
        swapChainCreateInfo.window = GetPlatformWindow();
        swapChainCreateInfo.presentQueue = graphicsQueue;
        swapChain = device->CreateSwapChain(&swapChainCreateInfo);
    }

    void CreateVertexBuffer()
    {
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

    void CreatePipelineLayout()
    {
        PipelineLayoutCreateInfo createInfo {};
        createInfo.bindGroupNum = 0;
        createInfo.bindGroupLayouts = nullptr;
        pipelineLayout = device->CreatePipelineLayout(&createInfo);
    }

    void CreatePipeline()
    {
        ShaderModule* vertexShader;
        ShaderModule* fragmentShader;

        std::vector<uint8_t> vsByteCode;
        CompileShader(vsByteCode, "Triangle.hlsl", "VSMain", RHI::ShaderStageBits::VERTEX);

        ShaderModuleCreateInfo shaderModuleCreateInfo {};
        shaderModuleCreateInfo.size = vsByteCode.size();
        shaderModuleCreateInfo.byteCode = vsByteCode.data();
        vertexShader = device->CreateShaderModule(&shaderModuleCreateInfo);

        std::vector<uint8_t> fsByteCode;
        CompileShader(fsByteCode, "Triangle.hlsl", "FSMain", RHI::ShaderStageBits::FRAGMENT);

        shaderModuleCreateInfo.size = fsByteCode.size();
        shaderModuleCreateInfo.byteCode = fsByteCode.data();
        fragmentShader = device->CreateShaderModule(&shaderModuleCreateInfo);

        std::array<VertexAttribute, 2> vertexAttributes {};
        vertexAttributes[0].format = VertexFormat::FLOAT32_X3;
        vertexAttributes[0].offset = 0;
        vertexAttributes[0].location = 0;
        vertexAttributes[1].format = VertexFormat::FLOAT32_X3;
        vertexAttributes[1].offset = 0;
        vertexAttributes[1].location = 1;

        VertexBufferLayout vertexBufferLayout {};
        vertexBufferLayout.stepMode = RHI::VertexStepMode::PER_VERTEX;
        vertexBufferLayout.stride = sizeof(Vertex);
        vertexBufferLayout.attributeNum = vertexAttributes.size();
        vertexBufferLayout.attributes = vertexAttributes.data();

        ColorTargetState colorTargetState {};
        colorTargetState.format = PixelFormat::RGBA8_UNORM;
        colorTargetState.writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;

        GraphicsPipelineCreateInfo createInfo {};
        createInfo.vertexShader = vertexShader;
        createInfo.fragmentShader = fragmentShader;
        createInfo.layout = pipelineLayout;
        createInfo.vertex.bufferLayoutNum = 1;
        createInfo.vertex.bufferLayouts = &vertexBufferLayout;
        createInfo.fragment.colorTargetNum = 1;
        createInfo.fragment.colorTargets = &colorTargetState;
        createInfo.primitive.depthClip = false;
        createInfo.primitive.frontFace = RHI::FrontFace::CCW;
        createInfo.primitive.cullMode = CullMode::NONE;
        createInfo.primitive.topology = RHI::PrimitiveTopology::TRIANGLE;
        createInfo.primitive.stripIndexFormat = IndexFormat::UINT16;
        createInfo.depthStencil.depthEnable = false;
        createInfo.depthStencil.stencilEnable = false;
        createInfo.multiSample.count = 1;
        pipeline = device->CreateGraphicsPipeline(&createInfo);
    }

    Instance* instance = nullptr;
    Gpu* gpu = nullptr;
    Device* device = nullptr;
    Queue* graphicsQueue = nullptr;
    SwapChain* swapChain = nullptr;
    Buffer* vertexBuffer = nullptr;
    BufferView* vertexBufferView = nullptr;
    PipelineLayout* pipelineLayout = nullptr;
    Pipeline* pipeline = nullptr;
};

int main(int argc, char* argv[])
{
    TriangleApplication application("RHI-Triangle");
    return application.Run(argc, argv);
}
