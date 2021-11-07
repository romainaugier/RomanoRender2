static inline int wang_hash(int seed)
{
    int s = seed;
    s = (s ^ 61u) ^ (s >> 16u);
    s *= 9u;
    s = s ^ (s >> 4u);
    s *= 0x27d4eb2du;
    s = s ^ (s >> 15u);
    return 1u + s;
}

static inline int xorshift32(int state)
{
    int x = state;
    x ^= x << 13u;
    x ^= x >> 17u;
    x ^= x << 5u;
    return x;
}

export void randomFloatWangHash(uniform int seeds[], 
                                uniform float randoms[], 
                                uniform float toFloat, 
                                uniform int count)
{
    foreach(i = 0 ... count)
    {
        int state = wang_hash(seeds[i]);
        int x = xorshift32(state);
        randoms[i] = (float)x * toFloat + 0.5f;
    }
}

export void randomFloatWangHash1(const uniform int seeds[], 
                                 uniform int state, 
                                 uniform float randoms[], 
                                 uniform float toFloat, 
                                 uniform int count)
{
    foreach(i = 0 ... count)
    {
        int seed = state * xorshift32(seeds[i]) * i;
        int y = wang_hash(seed);
        int x = xorshift32(y);
        randoms[i] = (float)x * toFloat + 0.5f;
    }
}