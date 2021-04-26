//
// Created by John Kindem on 2021/4/26.
//

#ifndef EXPLOSION_PIPELINE_H
#define EXPLOSION_PIPELINE_H

#include <vector>

#include <vulkan/vulkan.h>

namespace Explosion {
    class Driver;
    class Device;

    class Pipeline {
    public:
        ~Pipeline();

    protected:
        VkShaderModule CreateShaderModule(const std::vector<char>& code);
        void DestroyShaderModule(const VkShaderModule& shaderModule);

        explicit Pipeline(Driver& driver);

        Driver& driver;
        Device& device;
    };

    class GraphicsPipeline : public Pipeline {
    public:
        struct Config {

        };

        GraphicsPipeline(Driver& driver, const Config& config);
        ~GraphicsPipeline();

    private:
        void CreateGraphicsPipeline();
        void DestroyGraphicsPipeline();

        Config config {};
    };

    class ComputePipeline : public Pipeline {
    public:
        struct Config {

        };

        ComputePipeline(Driver& driver, const Config& config);
        ~ComputePipeline();

    private:
        Config config {};
    };
}

#endif //EXPLOSION_PIPELINE_H
