//
// Created by John Kindem on 2021/4/27.
//

#ifndef EXPLOSION_COMMANDENCODER_H
#define EXPLOSION_COMMANDENCODER_H

namespace Explosion {
    class Driver;
    class Device;
    class CommandBuffer;

    class CommandEncoder {
    public:
        CommandEncoder(Driver& driver, CommandBuffer* commandBuffer);
        ~CommandEncoder();

    private:
        Driver& driver;
        Device& device;
        CommandBuffer* commandBuffer = nullptr;
    };
}

#endif //EXPLOSION_COMMANDENCODER_H
