//
// Created by johnk on 2023/4/17.
//

#pragma once

#include <windows.h>

#include <RHI/Surface.h>

namespace RHI::DirectX12 {
    class DX12Surface : public Surface {
    public:
        NonCopyable(DX12Surface)
        explicit DX12Surface(const SurfaceCreateInfo& inCreateInfo);
        ~DX12Surface() override;

        HWND GetNative() const;

    private:
        HWND hWnd;
    };
}
