//
// Created by johnk on 2023/4/17.
//

#include <RHI/DirectX12/Surface.h>

namespace RHI::DirectX12 {
    DX12Surface::DX12Surface(const RHI::SurfaceCreateInfo& createInfo)
        : Surface(createInfo)
        , hWnd(static_cast<HWND>(createInfo.window))
    {
    }

    DX12Surface::~DX12Surface() = default;

    void DX12Surface::Destroy()
    {
        delete this;
    }

    HWND DX12Surface::GetWin32WindowHandle() const
    {
        return hWnd;
    }
}
