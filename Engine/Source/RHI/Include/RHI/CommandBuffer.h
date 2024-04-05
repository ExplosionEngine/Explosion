//
// Created by johnk on 21/2/2022.
//

#pragma once

#include <Common/Utility.h>

namespace RHI {
    class CommandRecorder;

    class CommandBuffer {
    public:
        NonCopyable(CommandBuffer)
        virtual ~CommandBuffer();

        virtual CommandRecorder* Begin() = 0;
        virtual void Destroy() = 0;

    protected:
        CommandBuffer();
    };
}
