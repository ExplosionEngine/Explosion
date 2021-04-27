//
// Created by John Kindem on 2021/4/27.
//

#include <Explosion/Driver/CommandEncoder.h>
#include <Explosion/Driver/Driver.h>

namespace Explosion {
    CommandEncoder::CommandEncoder(Driver& driver, CommandBuffer* commandBuffer)
        : driver(driver), device(*driver.GetDevice()), commandBuffer(commandBuffer) {}

    CommandEncoder::~CommandEncoder() = default;
}
