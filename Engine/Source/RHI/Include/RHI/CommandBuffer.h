//
// Created by johnk on 21/2/2022.
//

#pragma once

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    class CommandRecorder;

    class CommandBuffer {
    public:
        NonCopyable(CommandBuffer)
        virtual ~CommandBuffer();

        virtual Common::UniqueRef<CommandRecorder> Begin() = 0;

    protected:
        CommandBuffer();
    };
}
