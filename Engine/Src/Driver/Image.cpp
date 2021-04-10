//
// Created by John Kindem on 2021/4/10.
//

#include <Explosion/Driver/Image.h>

namespace Explosion {
    Image::Image(VkImage const& vkImage) : holder(false), vkImage(vkImage) {}

    Image::~Image() {}
}
