//
// Created by LiZhen on 2021/5/10.
//

#ifndef EXPLOSION_FGNODE_H
#define EXPLOSION_FGNODE_H

#include <string>
#include <Explosion/Common/NonCopy.h>

namespace Explosion {

    class FgNode : public URefObject<FgNode>, public NonCopy {
    public:
        explicit FgNode(const char* str) : name(str) {}
        ~FgNode() override = default;

        const std::string& GetName() const { return name; }

    private:
        std::string name;
    };

}

#endif //EXPLOSION_FGNODE_H
