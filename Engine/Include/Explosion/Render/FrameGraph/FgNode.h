//
// Created by LiZhen on 2021/5/10.
//

#ifndef EXPLOSION_FGNODE_H
#define EXPLOSION_FGNODE_H

#include <string>
#include <Explosion/Common/NonCopy.h>

namespace Explosion {

    class FgNode : public URefObject<FgNode> {
    public:
        explicit FgNode(const char* str) : name(str), sideEffect(false) {}
        ~FgNode() override = default;

        EXPLOSION_NON_COPY(FgNode);

        const std::string& GetName() const { return name; }

        void SideEffect() { sideEffect = true; }

        bool IsSideEffect() const { return sideEffect; }

        bool IsActive() const { return GetRef() != 0 || sideEffect;  }

    private:
        std::string name;
        bool sideEffect;
    };

}

#endif //EXPLOSION_FGNODE_H
