//
// Created by Zach Lee on 2022/4/4.
//

#pragma once
#include <stdexcept>
#include <string>

namespace RHI::Vulkan {
    class VKException : public std::exception {
    public:
        explicit VKException(std::string m) : msg(std::move(m)) {}
        ~VKException() override = default;

        [[nodiscard]] const char* what() const noexcept override
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}
