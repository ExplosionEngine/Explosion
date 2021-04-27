//
// Created by John Kindem on 2021/4/27.
//

#ifndef EXPLOSION_COMMANDENCODER_H
#define EXPLOSION_COMMANDENCODER_H

namespace Explosion {
    class Driver;
    class Device;

    class CommandEncoder {
    public:
        CommandEncoder(Driver& driver);
        ~CommandEncoder();

    private:
        Driver& driver;
        Device& device;
    };
}

#endif //EXPLOSION_COMMANDENCODER_H
