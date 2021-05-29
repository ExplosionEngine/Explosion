//
// Created by LiZhen on 2021/5/12.
//

#ifndef EXPLOSION_FGBLACKBOARD_H
#define EXPLOSION_FGBLACKBOARD_H

#include <unordered_map>
#include <string_view>

#include <Explosion/Render/FrameGraph/FgResources.h>

namespace Explosion {

    class FgBlackBoard {
    public:
        FgBlackBoard() = default;
        ~FgBlackBoard() = default;

        void Add(const std::string_view& name, FgHandle handle) { resourceMap.emplace(name, handle); }

        void Remove(const std::string_view& name) { resourceMap.erase(name); }

        FgHandle Get(const std::string_view& name) const
        {
            auto iter = resourceMap.find(name);
            if (iter == resourceMap.end()) {
                return FgHandle();
            }
            return iter->second;
        }

    private:
        std::unordered_map<std::string_view, FgHandle> resourceMap;
    };
}


#endif //EXPLOSION_FGBLACKBOARD_H
