//
// Created by johnk on 2023/4/17.
//

#include <RHI/DirectX12/Surface.h>

namespace RHI::DirectX12 {
    DX12Surface::DX12Surface(const SurfaceCreateInfo& inCreateInfo)
        : Surface(inCreateInfo)
        , hWnd(static_cast<HWND>(inCreateInfo.window))
    {
    }

    DX12Surface::~DX12Surface() = default;

    HWND DX12Surface::GetNative() const
    {
        return hWnd;
    }
}
