#version 460 core
out float fragColor;

layout (location = 0) uniform vec2 resolution;

layout (location = 1) uniform float scale_x;
layout (location = 2) uniform float scale_y;
layout (location = 3) uniform float lacunarity;
layout (location = 4) uniform int octaves;
//layout (location = 5) uniform float phase;
layout (location = 6) uniform float seed;


uint ihash1D(uint q)
{
    // hash by Hugo Elias, Integer Hash - I, 2017
    q = q * 747796405u + 2891336453u;
    q = (q << 13u) ^ q;
    return q * (q * q * 15731u + 789221u) + 1376312589u;
}

uvec2 ihash1D(uvec2 q)
{
    // hash by Hugo Elias, Integer Hash - I, 2017
    q = q * 747796405u + 2891336453u;
    q = (q << 13u) ^ q;
    return q * (q * q * 15731u + 789221u) + 1376312589u;
}

uvec4 ihash1D(uvec4 q)
{
    // hash by Hugo Elias, Integer Hash - I, 2017
    q = q * 747796405u + 2891336453u;
    q = (q << 13u) ^ q;
    return q * (q * q * 15731u + 789221u) + 1376312589u;
}

// @return Value of the noise, range: [0, 1]
float hash1D(float x)
{
    // based on: pcg by Mark Jarzynski: http://www.jcgt.org/published/0009/03/02/
    uint state = uint(x * 8192.0) * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return float((word >> 22u) ^ word) * (1.0 / float(0xffffffffu));;
}

// @return Value of the noise, range: [0, 1]
float hash1D(vec2 x)
{
    // hash by Inigo Quilez, Integer Hash - III, 2017
    uvec2 q = uvec2(x * 8192.0);
    q = 1103515245u * ((q >> 1u) ^ q.yx);
    uint n = 1103515245u * (q.x ^ (q.y >> 3u));
    return float(n) * (1.0 / float(0xffffffffu));
}

// @return Value of the noise, range: [0, 1]
float hash1D(vec3 x)
{
    // based on: pcg3 by Mark Jarzynski: http://www.jcgt.org/published/0009/03/02/
    uvec3 v = uvec3(x * 8192.0) * 1664525u + 1013904223u;
    v += v.yzx * v.zxy;
    v ^= v >> 16u;
    return float(v.x + v.y * v.z) * (1.0 / float(0xffffffffu));
}

// @return Value of the noise, range: [0, 1]
vec2 hash2D(vec2 x)
{
    // based on: Inigo Quilez, Integer Hash - III, 2017
    uvec4 q = uvec2(x * 8192.0).xyyx + uvec2(0u, 3115245u).xxyy;
    q = 1103515245u * ((q >> 1u) ^ q.yxwz);
    uvec2 n = 1103515245u * (q.xz ^ (q.yw >> 3u));
    return vec2(n) * (1.0 / float(0xffffffffu));
}

// generates 2 random numbers for the coordinate
vec2 betterHash2D(vec2 x)
{
    uvec2 q = uvec2(x);
    uint h0 = ihash1D(ihash1D(q.x) + q.y);
    uint h1 = h0 * 1933247u + ~h0 ^ 230123u;
    return vec2(h0, h1)  * (1.0 / float(0xffffffffu));
}

// generates a random number for each of the 4 cell corners
vec4 betterHash2D(vec4 cell)
{
    uvec4 i = uvec4(cell);
    uvec4 hash = ihash1D(ihash1D(i.xzxz) + i.yyww);
    return vec4(hash) * (1.0 / float(0xffffffffu));
}

// generates 2 random numbers for each of the 4 cell corners
void betterHash2D(vec4 cell, out vec4 hashX, out vec4 hashY)
{
    uvec4 i = uvec4(cell);
    uvec4 hash0 = ihash1D(ihash1D(i.xzxz) + i.yyww);
    uvec4 hash1 = ihash1D(hash0 ^ 1933247u);
    hashX = vec4(hash0) * (1.0 / float(0xffffffffu));
    hashY = vec4(hash1) * (1.0 / float(0xffffffffu));
}

// generates 2 random numbers for each of the 2D coordinates
vec4 betterHash2D(vec2 coords0, vec2 coords1)
{
    uvec4 i = uvec4(coords0, coords1);
    uvec4 hash = ihash1D(ihash1D(i.xz) + i.yw).xxyy;
    hash.yw = hash.yw * 1933247u + ~hash.yw ^ 230123u;
    return vec4(hash) * (1.0 / float(0xffffffffu));;
}

// generates 2 random numbers for each of the four 2D coordinates
void betterHash2D(vec4 coords0, vec4 coords1, out vec4 hashX, out vec4 hashY)
{
    uvec4 hash0 = ihash1D(ihash1D(uvec4(coords0.xz, coords1.xz)) + uvec4(coords0.yw, coords1.yw));
    uvec4 hash1 = hash0 * 1933247u + ~hash0 ^ 230123u;
    hashX = vec4(hash0) * (1.0 / float(0xffffffffu));
    hashY = vec4(hash1) * (1.0 / float(0xffffffffu));
}

// @note Can change to (faster to slower order): permuteHash2D, betterHash2D
// Each has a tradeoff between quality and speed, some may also experience artifacts for certain ranges and are not realiable.
#define multiHash2D betterHash2D


// the main noise interpolation function using a hermite polynomial
float noiseInterpolate(const in float x)
{
    float x2 = x * x;
    return x2 * x * (x * (x * 6.0 - 15.0) + 10.0);
}
vec2 noiseInterpolate(const in vec2 x)
{
    vec2 x2 = x * x;
    return x2 * x * (x * (x * 6.0 - 15.0) + 10.0);
}
vec3 noiseInterpolate(const in vec3 x)
{
    vec3 x2 = x * x;
    return x2 * x * (x * (x * 6.0 - 15.0) + 10.0);
}
vec4 noiseInterpolate(const in vec4 x)
{
    vec4 x2 = x * x;
    return x2 * x * (x * (x * 6.0 - 15.0) + 10.0);
}
vec2 noiseInterpolateDu(const in float x)
{
    float x2 = x * x;
    float u = x2 * x * (x * (x * 6.0 - 15.0) + 10.0);
    float du = 30.0 * x2 * (x * (x - 2.0) + 1.0);
    return vec2(u, du);
}
vec4 noiseInterpolateDu(const in vec2 x)
{
    vec2 x2 = x * x;
    vec2 u = x2 * x * (x * (x * 6.0 - 15.0) + 10.0);
    vec2 du = 30.0 * x2 * (x * (x - 2.0) + 1.0);
    return vec4(u, du);
}
void noiseInterpolateDu(const in vec3 x, out vec3 u, out vec3 du)
{
    vec3 x2 = x * x;
    u = x2 * x * (x * (x * 6.0 - 15.0) + 10.0);
    du = 30.0 * x2 * (x * (x - 2.0) + 1.0);
}

// 2D Value noise that returns two values.
// @param scale Number of tiles, must be an integer for tileable results, range: [2, inf]
// @param phase The phase for rotating the hash, range: [0, inf], default: 0.0
// @param seed Seed to randomize result, range: [0, inf]
// @return Value of the noise, range: [-1, 1]
vec2 multiNoise(vec4 pos, vec4 scale, float phase, vec2 seed)
{
    const float kPI2 = 6.2831853071;
    pos *= scale;
    vec4 i = floor(pos);
    vec4 f = pos - i;
    vec4 i0 = mod(i.xyxy + vec2(0.0, 1.0).xxyy, scale.xyxy) + seed.x;
    vec4 i1 = mod(i.zwzw + vec2(0.0, 1.0).xxyy, scale.xyxy) + seed.y;

    vec4 hash0 = multiHash2D(i0);
    hash0 = 0.5 * sin(phase + kPI2 * hash0) + 0.5;
    vec4 hash1 = multiHash2D(i1);
    hash1 = 0.5 * sin(phase + kPI2 * hash1) + 0.5;
    vec2 a = vec2(hash0.x, hash1.x);
    vec2 b = vec2(hash0.y, hash1.y);
    vec2 c = vec2(hash0.z, hash1.z);
    vec2 d = vec2(hash0.w, hash1.w);

    vec4 u = noiseInterpolate(f);
    vec2 value = mix(a, b, u.xz) + (c - a) * u.yw * (1.0 - u.xz) + (d - b) * u.xz * u.yw;
    return value * 2.0 - 1.0;
}

vec2 multiNoise(vec4 pos, vec4 scale, float time, float seed) {
    return multiNoise(pos, scale, time, vec2(seed));
}

// FBM implementation using value noise which returns multiple values.
// @param scale Number of tiles, must be  integer for tileable results, range: [2, inf]
// @param octaves Number of octaves for the fbm, range: [1, inf]
// @param lacunarity Frequency of the fbm, must be integer for tileable results, range: [1, 32]
// @param phase The phase for rotating the hash, range: [0, inf], default: 0.0
// @param seed Seed to randomize result, range: [0, inf], default: 0.0
// @return value of the noise, range: [0, inf]
vec4 fbmMulti(vec2 pos, vec2 scale, float lacunarity, int octaves, float phase, float seed)
{
    vec4 seeds = vec4(0.0, 1031.0, 537.0, 23.0) + seed;
    float f = 2.0 / lacunarity;

    vec4 value = vec4(0.0);
    float w = 1.0;
    float acc = 0.0;
    for (int i = 0; i < octaves; i++)
    {
        vec2 ns = vec2(scale / w);
        vec4 n;
        n.xy = multiNoise(pos.xyxy, ns.xyxy, phase, seeds.xy);
        n.zw = multiNoise(pos.xyxy, ns.xyxy, phase, seeds.zw);
        value += (n * 0.5 + 0.5) * w;
        acc += w;
        w *= 0.5 * f;
    }
    return value / acc;
}

void main() {
    vec2 uv = gl_FragCoord.xy / resolution;

//    float height = fbmMulti(uv, vec2(scale_x, scale_y), lacunarity, octaves, phase, seed).x;
    float height = fbmMulti(uv, vec2(scale_x, scale_y), lacunarity, octaves, 0.0f, seed).x;

    fragColor = height;
}
