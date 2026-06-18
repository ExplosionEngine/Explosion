//
// Created by johnk on 2026/6/19.
//

#pragma once

#include <cstdint>

#include <Common/Platform.h>

#if ARCH_X86
#include <emmintrin.h>
#elif ARCH_ARM
#include <arm_neon.h>
#else
#error "The math SIMD backend supports only x86-64 (SSE2) and arm64 (NEON) targets"
#endif

namespace Common {
    // The SIMD backend only relies on the SSE2/NEON baseline, available on every CPU this engine targets, so it is the
    // default; defaultBackend aliases simd and sits after max so it does not change the enumerator count. The scalar
    // backend can still be requested explicitly via Vec/Mat/Quaternion's backend parameter.
    enum class MathBackend : uint8_t {
        scalar,
        simd,
        max,
        defaultBackend = simd
    };
}

// A thin, header-only wrapper over a 4-wide float SIMD register, mapping to the SSE2 baseline on x86-64 and to NEON on
// arm64 (both always present, no extra flags). The math layer uses it to back the simd backend.
namespace Common::Simd {
#if ARCH_X86
    using F32x4 = __m128;

    inline F32x4 LoadU(const float* p) { return _mm_loadu_ps(p); }
    inline void StoreU(float* p, F32x4 v) { _mm_storeu_ps(p, v); }
    inline F32x4 Set1(float s) { return _mm_set1_ps(s); }
    inline F32x4 Add(F32x4 a, F32x4 b) { return _mm_add_ps(a, b); }
    inline F32x4 Sub(F32x4 a, F32x4 b) { return _mm_sub_ps(a, b); }
    inline F32x4 Mul(F32x4 a, F32x4 b) { return _mm_mul_ps(a, b); }
    inline F32x4 Div(F32x4 a, F32x4 b) { return _mm_div_ps(a, b); }

    inline float Sum(F32x4 v)
    {
        F32x4 shuf = _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 3, 0, 1));
        F32x4 sums = _mm_add_ps(v, shuf);
        shuf = _mm_movehl_ps(shuf, sums);
        sums = _mm_add_ss(sums, shuf);
        return _mm_cvtss_f32(sums);
    }

    inline F32x4 Set(float x, float y, float z, float w) { return _mm_set_ps(w, z, y, x); }

    template <int L>
    inline F32x4 Splat(F32x4 v) { return _mm_shuffle_ps(v, v, _MM_SHUFFLE(L, L, L, L)); }

    template <int I0, int I1, int I2, int I3>
    inline F32x4 Shuffle(F32x4 v) { return _mm_shuffle_ps(v, v, _MM_SHUFFLE(I3, I2, I1, I0)); }

    // In-place transpose of the 4x4 matrix whose rows are r0..r3.
    inline void Transpose4(F32x4& r0, F32x4& r1, F32x4& r2, F32x4& r3)
    {
        _MM_TRANSPOSE4_PS(r0, r1, r2, r3);
    }
#elif ARCH_ARM
    using F32x4 = float32x4_t;

    inline F32x4 LoadU(const float* p) { return vld1q_f32(p); }
    inline void StoreU(float* p, F32x4 v) { vst1q_f32(p, v); }
    inline F32x4 Set1(float s) { return vdupq_n_f32(s); }
    inline F32x4 Add(F32x4 a, F32x4 b) { return vaddq_f32(a, b); }
    inline F32x4 Sub(F32x4 a, F32x4 b) { return vsubq_f32(a, b); }
    inline F32x4 Mul(F32x4 a, F32x4 b) { return vmulq_f32(a, b); }
    inline F32x4 Div(F32x4 a, F32x4 b) { return vdivq_f32(a, b); }
    inline float Sum(F32x4 v) { return vaddvq_f32(v); }

    inline F32x4 Set(float x, float y, float z, float w)
    {
        const float values[4] = { x, y, z, w };
        return vld1q_f32(values);
    }

    template <int L>
    inline F32x4 Splat(F32x4 v) { return vdupq_n_f32(vgetq_lane_f32(v, L)); }

    // NEON has no single arbitrary 4-lane float shuffle, so go through memory; for a compile-time permutation the
    // compiler routinely folds the store/reload back into register moves.
    template <int I0, int I1, int I2, int I3>
    inline F32x4 Shuffle(F32x4 v)
    {
        float tmp[4];
        vst1q_f32(tmp, v);
        const float out[4] = { tmp[I0], tmp[I1], tmp[I2], tmp[I3] };
        return vld1q_f32(out);
    }

    // In-place transpose of the 4x4 matrix whose rows are r0..r3.
    inline void Transpose4(F32x4& r0, F32x4& r1, F32x4& r2, F32x4& r3)
    {
        const float32x4x2_t t01 = vtrnq_f32(r0, r1);
        const float32x4x2_t t23 = vtrnq_f32(r2, r3);
        r0 = vcombine_f32(vget_low_f32(t01.val[0]), vget_low_f32(t23.val[0]));
        r1 = vcombine_f32(vget_low_f32(t01.val[1]), vget_low_f32(t23.val[1]));
        r2 = vcombine_f32(vget_high_f32(t01.val[0]), vget_high_f32(t23.val[0]));
        r3 = vcombine_f32(vget_high_f32(t01.val[1]), vget_high_f32(t23.val[1]));
    }
#endif

    // Safe partial load/store for tight 3-float storage (a Vec3, or one row of a Mat3). Load3 reads exactly three
    // floats and zeroes the 4th lane, so it never over-reads the float[3] / float[9] backing; the zeroed lane also lets
    // a 4-wide dot reduce to the 3-component dot. Store3 writes exactly three floats and leaves the 4th element alone.
    inline F32x4 Load3(const float* p) { return Set(p[0], p[1], p[2], 0.0f); }

    inline void Store3(float* p, F32x4 v)
    {
        alignas(16) float tmp[4];
        StoreU(tmp, v);
        p[0] = tmp[0];
        p[1] = tmp[1];
        p[2] = tmp[2];
    }

    // Element-wise binary ops as functors so a single Map* template can drive every Vec/Mat/Quaternion kernel. Each
    // carries both a 4-wide register overload (the SIMD body) and a scalar overload (the <4 tail), so the same functor
    // covers the full chunks and the remainder without the caller spelling out two lambdas.
    struct AddOp {
        F32x4 operator()(F32x4 a, F32x4 b) const { return Add(a, b); }
        float operator()(float a, float b) const { return a + b; }
    };

    struct SubOp {
        F32x4 operator()(F32x4 a, F32x4 b) const { return Sub(a, b); }
        float operator()(float a, float b) const { return a - b; }
    };

    struct MulOp {
        F32x4 operator()(F32x4 a, F32x4 b) const { return Mul(a, b); }
        float operator()(float a, float b) const { return a * b; }
    };

    struct DivOp {
        F32x4 operator()(F32x4 a, F32x4 b) const { return Div(a, b); }
        float operator()(float a, float b) const { return a / b; }
    };

    // Lane-wise binary map over N contiguous floats: full 4-wide chunks via the register overload, any <4 tail via the
    // scalar overload. dst may alias a or b since each lane is independent, so this also backs the in-place compound ops.
    template <int N, typename Op>
    inline void MapBinary(float* dst, const float* a, const float* b, Op op = {})
    {
        int i = 0;
        for (; i + 4 <= N; i += 4) { StoreU(&dst[i], op(LoadU(&a[i]), LoadU(&b[i]))); }
        for (; i < N; i++) { dst[i] = op(a[i], b[i]); }
    }

    // As MapBinary, but the right operand is a single scalar broadcast across every lane (and applied directly in the
    // scalar tail).
    template <int N, typename Op>
    inline void MapScalar(float* dst, const float* a, float b, Op op = {})
    {
        const F32x4 bv = Set1(b);
        int i = 0;
        for (; i + 4 <= N; i += 4) { StoreU(&dst[i], op(LoadU(&a[i]), bv)); }
        for (; i < N; i++) { dst[i] = op(a[i], b); }
    }
}
