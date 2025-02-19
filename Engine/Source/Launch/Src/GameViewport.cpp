//
// Created by johnk on 2025/2/19.
//

#include <Launch/GameViewport.h>

namespace Launch {
    GameViewport::GameViewport()
    {
        // TODO
    }

    GameViewport::~GameViewport()
    {
        // TODO
    }

    Runtime::Client& GameViewport::GetClient()
    {
        // TODO
        return *static_cast<Runtime::Client*>(nullptr);
    }

    Runtime::PresentInfo GameViewport::GetNextPresentInfo()
    {
        // TODO
        return Runtime::PresentInfo();
    }

    size_t GameViewport::GetWidth()
    {
        // TODO
        return 0;
    }

    size_t GameViewport::GetHeight()
    {
        // TODO
        return 0;
    }
}
