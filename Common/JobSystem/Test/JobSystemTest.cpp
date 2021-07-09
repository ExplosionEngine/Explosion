//
// Created by Administrator on 2021/6/12 0012.
//

#include <gtest/gtest.h>

#include <JobSystem/JobSystem.h>

using namespace Explosion::JobSystem;

TEST(JobSystemTest, SimpleTaskTest)
{
    std::vector<uint32_t> inputs(10);
    for (uint32_t i = 0; i < 10; i++) {
        inputs[i] = i;
    }

    TaskFlow taskFlow;
    std::vector<uint32_t> result(10);
    for (uint32_t i = 0; i < 10; i++) {
        taskFlow.emplace([&inputs, &result, i]() -> void {
            result[i] = inputs[i] * 10;
        });
    }

    Executor executor;
    executor.run(taskFlow).wait();

    for (uint32_t i = 0; i < 10; i++) {
        ASSERT_EQ(result[i], inputs[i] * 10);
    }
}

TEST(JobSystemTest, DependencyTest)
{
    uint32_t sharedMemory = 10;

    TaskFlow taskFlow;
    auto [task1, task2] = taskFlow.emplace(
        [&sharedMemory]() -> void { sharedMemory *= 2; },
        [&sharedMemory]() -> void { sharedMemory *= 3; }
    );
    task1.succeed(task2);

    Executor executor;
    executor.run(taskFlow).wait();

    ASSERT_EQ(sharedMemory, 60);
}
