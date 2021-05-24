//
// Created by John Kindem on 2021/5/16 0016.
//

#include <Explosion/RHI/Common/Image.h>

namespace Explosion::RHI {
    Image::Image(Config config) : config(config) {}

    Image::~Image() = default;

    const Image::Config& Image::GetConfig()
    {
        return config;
    }
}
