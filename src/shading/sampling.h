#pragma once

#include "../common/math/Vec3.h"
#include "../common/math/constants.h"
#include <immintrin.h>
#include <limits>
#include <cstring>

__forceinline float deg2rad(const float deg) { return deg * float(embree::pi) / 180; }
__forceinline float rad2deg(const float rad) { return rad * 180 / float(embree::pi); }

template <typename T>
__forceinline T fit(T s, T a1, T a2, T b1, T b2) { return b1 + ((s - a1) * (b2 - b1)) / (a2 - a1); }

template <typename T>
__forceinline T fit01(T x, T a, T b) { return x * (b - a) + a; }

template <typename T>
__forceinline T lerp(T a, T b, float t) { return (1 - t) * a + t * b; }

template <typename T>
__forceinline T clamp(T n, float lower, float upper) { return fmax(lower, fmin(n, upper)); }

__forceinline int wang_hash(int seed) noexcept
{
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4u);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15u);
    return 1u + seed;
}


__forceinline void wang_hash4(int* seed) noexcept
{
    for (int i = 0; i < 4; i++)
    {
        seed[i] = (seed[i] ^ 61u) ^ (seed[i] >> 16u);
        seed[i] *= 9u;
        seed[i] = seed[i] ^ (seed[i] >> 4u);
        seed[i] *= 0x27d4eb2du;
        seed[i] = seed[i] ^ (seed[i] >> 15u);
        seed[i] += 1u;
    }
}


__forceinline void wang_hash8(int* seed) noexcept
{
    for (int i = 0; i < 8; i++)
    {
        seed[i] = (seed[i] ^ 61u) ^ (seed[i] >> 16u);
        seed[i] *= 9u;
        seed[i] = seed[i] ^ (seed[i] >> 4u);
        seed[i] *= 0x27d4eb2du;
        seed[i] = seed[i] ^ (seed[i] >> 15u);
        seed[i] += 1u;
    }
}


__forceinline int xorshift32(int state) noexcept
{
    int x = state;
    x ^= x << 13u;
    x ^= x >> 17u;
    x ^= x << 5u;
    return x;
}


__forceinline void xorshift324(int* state) noexcept
{
    for (int i = 0; i < 4; i++)
    {
        int x = state[i];
        x ^= x << 13u;
        x ^= x >> 17u;
        x ^= x << 5u;
        state[i] = x;
    }
}


__forceinline void xorshift328(int* state) noexcept
{
    for (int i = 0; i < 8; i++)
    {
        int x = state[i];
        x ^= x << 13u;
        x ^= x >> 17u;
        x ^= x << 5u;
        state[i] = x;
    }
}


inline float generate_random_float_slow() noexcept
{
    constexpr unsigned int floatAddr = 0x2f800004u;
    auto toFloat = float();
    memcpy(&toFloat, &floatAddr, 4);

    static unsigned long x = 123456789, y = 362436069, z = 521288629;

    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    float a = static_cast<float>(z) * toFloat + 0.5f;

    return a;
}


inline float randomFloatWangHash(int state) noexcept
{
    constexpr unsigned int floatAddr = 0x2f800004u;
    auto toFloat = float();
    memcpy(&toFloat, &floatAddr, 4);

    state = wang_hash(state);
    int x = xorshift32(state);
    state = x;
    return static_cast<float>(x) * toFloat + 0.5f;
}


inline float* randomFloatWangHash4(int* state) noexcept
{
    constexpr unsigned int tofloat = 0x2f800004u;
    wang_hash4(state);
    xorshift324(state);

    float randoms[4];
    
    for (int i = 0; i < 4; i++)
    {
        randoms[i] = static_cast<float>(state[i]) * reinterpret_cast<const float&>(tofloat) + 0.5f;
    }

    return randoms;
}


inline void randomFloatWangHash8(float* randoms, int* state) noexcept
{
    constexpr unsigned int floatAddr = 0x2f800004u;
    auto toFloat = float();
    memcpy(&toFloat, &floatAddr, 4);

    wang_hash8(state);
    xorshift328(state);

    for (int i = 0; i < 8; i++)
    {
        randoms[i] = static_cast<float>(state[i]) * toFloat + 0.5f;
    }
}


// The following two random float generation functions are from
// https://www.reedbeta.com/blog/hash-functions-for-gpu-rendering/

inline float randomFloatPcg(unsigned int state)
{
    constexpr unsigned int floatAddr = 0x2f800004u;
    auto toFloat = float();
    memcpy(&toFloat, &floatAddr, 4);

    unsigned int state2 = state * 747796405u + 2891336453u;
    unsigned int word = ((state2 >> ((state2 >> 28u) + 4u)) ^ state2) * 277803737u;
    state = (word >> 22u) ^ word;

    return static_cast<float>(state) * toFloat;
}


inline void randomFloatPcg8(float* randoms, unsigned int* state)
{
    __m256i states = _mm256_set1_epi32(94853);
    const __m256i constant1 = _mm256_set1_epi32(747796405u);
    const __m256i constant2 = _mm256_set1_epi32(2891336453u);
    const __m256i constant3 = _mm256_set1_epi32(277803737u);

    states = _mm256_add_epi32(_mm256_mul_epi32(states, constant1), constant2);

    const __m256i word = _mm256_mul_epi32(
        _mm256_xor_si256(
            _mm256_srav_epi32(states,
                _mm256_add_epi32(
                    _mm256_srav_epi32(states,
                        _mm256_set1_epi32(28u)),
                    _mm256_set1_epi32(4u))),
            states),
        constant3);

    states = _mm256_xor_si256(
        _mm256_srav_epi32(word,
            _mm256_set1_epi32(22u)),
        word);

    constexpr unsigned int floatAddr = 0x2f800004u;
    auto toFloat = float();
    memcpy(&toFloat, &floatAddr, 4);

    const __m256 tofloat2 = _mm256_set1_ps(toFloat);
    __m256 floats = _mm256_mul_ps(_mm256_cvtepi32_ps(states), tofloat2);
    floats = _mm256_add_ps(floats, _mm256_set1_ps(0.5f));
    _mm256_storeu_ps(randoms, floats);
}

inline embree::Vec3f SampleHemisphere(const embree::Vec3f& hit_normal, const float random_x, const float random_y, const float random_z)
{
    float signZ = (hit_normal.z >= 0.0f) ? 1.0f : -1.0f;
    float a = -1.0f / (signZ + hit_normal.z);
    float b = hit_normal.x * hit_normal.y * a;
    embree::Vec3f b1 = embree::Vec3f(1.0f + signZ * hit_normal.x * hit_normal.x * a, signZ * b, -signZ * hit_normal.x);
    embree::Vec3f b2 = embree::Vec3f(b, signZ + hit_normal.y * hit_normal.y * a, -hit_normal.y);

    float phi = 2.0f * float(embree::pi) * random_x;
    float cosTheta = embree::sqrt(random_y);
    float sinTheta = embree::sqrt(1.0f - random_z);

    return embree::normalize(((b1 * embree::cos(phi) + b2 * embree::sin(phi)) * cosTheta + hit_normal * sinTheta));
}

inline embree::Vec3f SampleHemisphereUnsafe(const embree::Vec3f& hit_normal, const unsigned int seed)
{
    float a = 1.0f - 2.0f * randomFloatWangHash(seed + 538239);
    float b = embree::sqrt(1.0f - a * a);
    float phi = 2.0f * float(embree::pi) *randomFloatWangHash(seed + 781523);

    return embree::Vec3f(hit_normal.x + b * embree::cos(phi), hit_normal.y + b * embree::sin(phi), hit_normal.z + a);
}
