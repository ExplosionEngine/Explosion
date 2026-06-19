//
// Created by johnk on 2026/6/19.
//

#include <random>
#include <vector>

#include <benchmark/benchmark.h>

#include <Common/Math/Vector.h>
#include <Common/Math/Matrix.h>
#include <Common/Math/Quaternion.h>

using namespace Common;

// A single 4-wide op (one Vec add, one dot) is latency-bound and, for fixed-size loops, the compiler already
// auto-vectorizes the scalar backend, so an isolated op shows no SIMD delta. Worse, with compile-time-constant inputs
// the whole computation is constant-folded and hoisted out of the loop, so a single-op benchmark would measure only a
// DoNotOptimize store. These benchmarks instead run each op over a runtime-randomized batch (inputs the optimizer can
// not fold, output consumed via DoNotOptimize/ClobberMemory) and report items/s, which is the throughput metric where
// SIMD's lane width actually shows up.
namespace {
    constexpr int batchSize = 1024;

    std::vector<float> MakeRandomFloats(const size_t count)
    {
        std::mt19937 rng(0x1234u);
        std::uniform_real_distribution<float> dist(0.5f, 1.5f);
        std::vector<float> values(count);
        for (auto& value : values) {
            value = dist(rng);
        }
        return values;
    }

    template <MathBackend B>
    std::vector<Vec<float, 4, B>> MakeRandomVecs(const size_t count)
    {
        const auto raw = MakeRandomFloats(count * 4);
        std::vector<Vec<float, 4, B>> result(count);
        for (size_t i = 0; i < count; i++) {
            result[i] = Vec<float, 4, B>(raw[i * 4 + 0], raw[i * 4 + 1], raw[i * 4 + 2], raw[i * 4 + 3]);
        }
        return result;
    }

    template <MathBackend B>
    std::vector<Mat<float, 4, 4, B>> MakeRandomMats(const size_t count)
    {
        const auto raw = MakeRandomFloats(count * 16);
        std::vector<Mat<float, 4, 4, B>> result(count);
        for (size_t i = 0; i < count; i++) {
            const float* p = &raw[i * 16];
            result[i] = Mat<float, 4, 4, B>(
                p[0], p[1], p[2], p[3],
                p[4], p[5], p[6], p[7],
                p[8], p[9], p[10], p[11],
                p[12], p[13], p[14], p[15]);
        }
        return result;
    }

    template <MathBackend B>
    std::vector<Quaternion<float, B>> MakeRandomQuats(const size_t count)
    {
        const auto raw = MakeRandomFloats(count * 4);
        std::vector<Quaternion<float, B>> result(count);
        for (size_t i = 0; i < count; i++) {
            result[i] = Quaternion<float, B>(raw[i * 4 + 0], raw[i * 4 + 1], raw[i * 4 + 2], raw[i * 4 + 3]);
        }
        return result;
    }

    template <MathBackend B>
    std::vector<Vec<float, 3, B>> MakeRandomVec3s(const size_t count)
    {
        const auto raw = MakeRandomFloats(count * 3);
        std::vector<Vec<float, 3, B>> result(count);
        for (size_t i = 0; i < count; i++) {
            result[i] = Vec<float, 3, B>(raw[i * 3 + 0], raw[i * 3 + 1], raw[i * 3 + 2]);
        }
        return result;
    }

    template <MathBackend B>
    std::vector<Mat<float, 3, 3, B>> MakeRandomMat3s(const size_t count)
    {
        const auto raw = MakeRandomFloats(count * 9);
        std::vector<Mat<float, 3, 3, B>> result(count);
        for (size_t i = 0; i < count; i++) {
            const float* p = &raw[i * 9];
            result[i] = Mat<float, 3, 3, B>(
                p[0], p[1], p[2],
                p[3], p[4], p[5],
                p[6], p[7], p[8]);
        }
        return result;
    }
}

template <MathBackend B>
static void VecAddBatch(benchmark::State& state)
{
    const auto a = MakeRandomVecs<B>(batchSize);
    const auto b = MakeRandomVecs<B>(batchSize);
    std::vector<Vec<float, 4, B>> c(batchSize);
    for (auto _ : state) {
        for (int i = 0; i < batchSize; i++) {
            c[i] = a[i] + b[i];
        }
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * batchSize);
}
BENCHMARK(VecAddBatch<MathBackend::scalar>);
BENCHMARK(VecAddBatch<MathBackend::simd>);

template <MathBackend B>
static void VecDotBatch(benchmark::State& state)
{
    const auto a = MakeRandomVecs<B>(batchSize);
    const auto b = MakeRandomVecs<B>(batchSize);
    for (auto _ : state) {
        float sum = 0.0f;
        for (int i = 0; i < batchSize; i++) {
            sum += a[i].Dot(b[i]);
        }
        benchmark::DoNotOptimize(sum);
    }
    state.SetItemsProcessed(state.iterations() * batchSize);
}
BENCHMARK(VecDotBatch<MathBackend::scalar>);
BENCHMARK(VecDotBatch<MathBackend::simd>);

template <MathBackend B>
static void MatMulBatch(benchmark::State& state)
{
    const auto a = MakeRandomMats<B>(batchSize);
    const auto b = MakeRandomMats<B>(batchSize);
    std::vector<Mat<float, 4, 4, B>> c(batchSize);
    for (auto _ : state) {
        for (int i = 0; i < batchSize; i++) {
            c[i] = a[i] * b[i];
        }
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * batchSize);
}
BENCHMARK(MatMulBatch<MathBackend::scalar>);
BENCHMARK(MatMulBatch<MathBackend::simd>);

// QuatOps<float, simd>::Mul evaluates the Hamilton product as four broadcast-and-permute terms, so this measures the
// SIMD quaternion product against the scalar one rather than a tie.
template <MathBackend B>
static void QuatMulBatch(benchmark::State& state)
{
    const auto a = MakeRandomQuats<B>(batchSize);
    const auto b = MakeRandomQuats<B>(batchSize);
    std::vector<Quaternion<float, B>> c(batchSize);
    for (auto _ : state) {
        for (int i = 0; i < batchSize; i++) {
            c[i] = a[i] * b[i];
        }
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * batchSize);
}
BENCHMARK(QuatMulBatch<MathBackend::scalar>);
BENCHMARK(QuatMulBatch<MathBackend::simd>);

// Mat3 keeps its tight float[9] storage; the simd backend loads it with safe partial loads (two full 128-bit loads
// plus a Load3 tail). These batches show whether that 2b approach beats the scalar 3x3 paths once the per-op load cost
// is amortized across the matrix product / transform.
template <MathBackend B>
static void Mat4InverseBatch(benchmark::State& state)
{
    const auto a = MakeRandomMats<B>(batchSize);
    std::vector<Mat<float, 4, 4, B>> c(batchSize);
    for (auto _ : state) {
        for (int i = 0; i < batchSize; i++) {
            c[i] = a[i].Inverse();
        }
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * batchSize);
}
BENCHMARK(Mat4InverseBatch<MathBackend::scalar>);
BENCHMARK(Mat4InverseBatch<MathBackend::simd>);

template <MathBackend B>
static void Mat3MulBatch(benchmark::State& state)
{
    const auto a = MakeRandomMat3s<B>(batchSize);
    const auto b = MakeRandomMat3s<B>(batchSize);
    std::vector<Mat<float, 3, 3, B>> c(batchSize);
    for (auto _ : state) {
        for (int i = 0; i < batchSize; i++) {
            c[i] = a[i] * b[i];
        }
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * batchSize);
}
BENCHMARK(Mat3MulBatch<MathBackend::scalar>);
BENCHMARK(Mat3MulBatch<MathBackend::simd>);

template <MathBackend B>
static void Mat3MulVecBatch(benchmark::State& state)
{
    const auto m = MakeRandomMat3s<B>(batchSize);
    const auto v = MakeRandomVec3s<B>(batchSize);
    std::vector<Vec<float, 3, B>> c(batchSize);
    for (auto _ : state) {
        for (int i = 0; i < batchSize; i++) {
            c[i] = m[i] * v[i];
        }
        benchmark::DoNotOptimize(c.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * batchSize);
}
BENCHMARK(Mat3MulVecBatch<MathBackend::scalar>);
BENCHMARK(Mat3MulVecBatch<MathBackend::simd>);
