//
// Created by johnk on 13/1/2022.
//

#include <RHI/DirectX12/Common.h>

namespace RHI::DirectX12 {
    GpuType GetGpuTypeByAdapterFlag(UINT flag)
    {
        return flag & DXGI_ADAPTER_FLAG_SOFTWARE ? GpuType::SOFTWARE : GpuType::HARDWARE;
    }
}
