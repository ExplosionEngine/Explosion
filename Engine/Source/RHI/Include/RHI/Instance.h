//
// Created by johnk on 9/1/2022.
//

#pragma once

#include <cstdint>

#include <Common/Utility.h>

#include <RHI/Common.h>

namespace RHI {
    class Gpu;

    RHIType GetPlatformRHIType();
    std::string GetPlatformDefaultRHIAbbrString();
    RHIType RHIAbbrStringToRHIType(const std::string& abbrString);
    std::string GetRHIModuleNameByType(RHIType type);

    class Instance {
    public:
        static Instance* GetByPlatform();
        static Instance* GetByType(const RHIType& type);
        static void UnloadByType(const RHIType& type);
        static void UnloadAllInstances();

        NonCopyable(Instance)
        virtual ~Instance();
        virtual RHIType GetRHIType() = 0;
        virtual uint32_t GetGpuNum() = 0;
        virtual Gpu* GetGpu(uint32_t index) = 0;
        virtual void Destroy() = 0;

    protected:
        explicit Instance();
    };
}
