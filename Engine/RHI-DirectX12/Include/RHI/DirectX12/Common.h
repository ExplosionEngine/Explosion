//
// Created by johnk on 10/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_COMMON_H
#define EXPLOSION_RHI_DX12_COMMON_H

#include <stdexcept>
#include <utility>

namespace RHI::DirectX12 {
    class DX12Exception : public std::exception {
    public:
        explicit DX12Exception(std::string msg) : message(std::move(msg)) {}

        [[nodiscard]] const char* what() const override
        {
            return message.c_str();
        }

    private:
        std::string message;
    };
}

#endif //EXPLOSION_RHI_DX12_COMMON_H
