//
// Created by John Kindem on 2021/1/25 0025.
//

#ifndef EXPLOSION_PIPELINE_H
#define EXPLOSION_PIPELINE_H

namespace Explosion {
    // Note: a material is associated with a pipeline
    class Pipeline {
    public:
        struct Descriptor {};

        virtual ~Pipeline() = 0;

    protected:
        Pipeline();
    };
}

#endif //EXPLOSION_PIPELINE_H
