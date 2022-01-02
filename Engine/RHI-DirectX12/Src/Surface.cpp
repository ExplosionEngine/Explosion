//
// Created by johnk on 1/1/2022.
//

#include <RHI/Surface.h>
#include <RHI/DirectX12/Surface.h>
#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/Utility.h>

namespace RHI::DirectX12 {
    DX12Surface::DX12Surface(DX12Instance& instance, const SurfaceCreateInfo* createInfo) : Surface(createInfo)
    {
        CreateSurface(instance, createInfo);
    }

    DX12Surface::~DX12Surface() = default;

    HWND DX12Surface::GetHWND()
    {
        return hWnd;
    }

    void DX12Surface::CreateSurface(DX12Instance& instance, const SurfaceCreateInfo* createInfo)
    {
        auto& property = instance.GetProperty();
        if (!(property.supportSurface && property.supportWindowsSurface)) {
            throw DX12Exception("must enable surface and windows surface extension when create instance");
        }

        if (createInfo->windows == nullptr) {
            throw DX12Exception("must fill windows create info in surface create info");
        }
        hWnd = static_cast<HWND>(createInfo->windows->hWnd);
    }
}
