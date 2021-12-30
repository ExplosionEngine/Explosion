//
// Created by johnk on 30/12/2021.
//

#ifndef EXPLOSION_RHI_DX12_UTILITY_H
#define EXPLOSION_RHI_DX12_UTILITY_H

#include <stdexcept>
#include <utility>

#include <winnt.h>
#include <winerror.h>

namespace RHI::DirectX12 {
    class DX12Exception : public std::exception {
    public:
        explicit DX12Exception(std::string e) : err(std::move(e)) {}

        [[nodiscard]] const char* what() const override
        {
            return err.c_str();
        }

    private:
        std::string err;
    };

    inline void ThrowIfFailed(HRESULT result, std::string e)
    {
        if (FAILED(result)) {
            throw DX12Exception(std::move(e));
        }
    }
}

#endif //EXPLOSION_RHI_DX12_UTILITY_H
