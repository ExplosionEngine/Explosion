//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_FGRENDERPASS_H
#define EXPLOSION_FGRENDERPASS_H

#include <functional>
#include <string>
#include <Explosion/Common/NonCopy.h>
#include <Explosion/Render/FrameGraph/FgResources.h>
#include <Explosion/Driver/Driver.h>

namespace Explosion {
    class FrameGraphBuilder;

    class FgRenderPassBase : public NonCopy {
    public:
        explicit FgRenderPassBase(const std::string& n) : name(n) {}

        ~FgRenderPassBase() override = default;

        virtual void Execute(const FgResources&, Driver&) = 0;
    private:
        std::string name;
    };

    template<typename Data>
    class FgRenderPass : public FgRenderPassBase {
    public:
        using ExecuteType = std::function<void(const Data &, const FgResources &, Driver&)>;

        FgRenderPass(const std::string& name, ExecuteType &&e) : FgRenderPassBase(name), execute(std::move(e)) {}

        ~FgRenderPass() override = default;

        void Execute(const FgResources &res, Driver& driver) override {
            if (execute) execute(data, res, driver);
        }

    private:
        friend class FrameGraph;
        ExecuteType execute;
        Data data;
    };
}

#endif //EXPLOSION_FGRENDERPASS_H
