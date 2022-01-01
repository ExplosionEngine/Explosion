//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_SURFACE_H
#define EXPLOSION_RHI_DX12_SURFACE_H

#include <windows.h>

#include <RHI/Surface.h>

namespace RHI::DirectX12 {
    class DX12Instance;

    class DX12Surface : public Surface {
    public:
        NON_COPYABLE(DX12Surface)
        explicit DX12Surface(DX12Instance& instance, const SurfaceCreateInfo& createInfo);
        ~DX12Surface() override;

        HWND GetHWND();

    private:
        void CreateSurface(DX12Instance& instance, const SurfaceCreateInfo& createInfo);

        HWND hWnd{};
    };
}

#endif //EXPLOSION_RHI_DX12_SURFACE_H
