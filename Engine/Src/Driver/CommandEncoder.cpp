//
// Created by John Kindem on 2021/4/27.
//

#include <Explosion/Driver/CommandEncoder.h>
#include <Explosion/Driver/Driver.h>
#include <Explosion/Driver/GpuBuffer.h>
#include <Explosion/Driver/CommandBuffer.h>

namespace Explosion {
    CommandEncoder::CommandEncoder(Driver& driver, CommandBuffer* commandBuffer)
        : driver(driver), device(*driver.GetDevice()), commandBuffer(commandBuffer) {}

    CommandEncoder::~CommandEncoder() = default;

    void CommandEncoder::CopyBuffer(GpuBuffer* srcBuffer, GpuBuffer* dstBuffer)
    {
        VkBufferCopy bufferCopy {};
        bufferCopy.srcOffset = 0;
        bufferCopy.dstOffset = 0;
        bufferCopy.size = srcBuffer->GetSize();
        vkCmdCopyBuffer(commandBuffer->GetVkCommandBuffer(), srcBuffer->GetVkBuffer(), dstBuffer->GetVkBuffer(), 1, &bufferCopy);
    }
}
