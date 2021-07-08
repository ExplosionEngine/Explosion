//
// Created by John Kindem on 2021/6/11.
//

#ifndef EXPLOSION_JOBSYSTEM_H
#define EXPLOSION_JOBSYSTEM_H

#include <taskflow/taskflow.hpp>

namespace Explosion::JobSystem {
    using Task = tf::Task;
    using TaskFlow = tf::Taskflow;
    using Executor = tf::Executor;
}

#endif //EXPLOSION_JOBSYSTEM_H
