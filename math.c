#include <math.h>

#include "math.h"

vec2 vec2_add(vec2 a, vec2 b)
{
    vec2 r = {a.x + b.x, a.y + b.y};
    return r;
}

vec2 vec2_sub(vec2 a, vec2 b)
{
    vec2 r = {a.x - b.x, a.y - b.y};
    return r;
}

vec2 vec2_scale(vec2 v, f32 s)
{
    vec2 r = {v.x * s, v.y * s};
    return r;
}

vec3 vec3_add(vec3 a, vec3 b)
{
    vec3 r = {a.x + b.x, a.y + b.y, a.z + b.z};
    return r;
}

vec3 vec3_sub(vec3 a, vec3 b)
{
    vec3 r = {a.x - b.x, a.y - b.y, a.z - b.z};
    return r;
}

vec3 vec3_scale(vec3 v, f32 s)
{
    vec3 r = {v.x * s, v.y * s, v.z * s};
    return r;
}

f32 vec3_dot(vec3 a, vec3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 vec3_cross(vec3 a, vec3 b)
{
    vec3 r = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
    };
    return r;
}

vec3 vec3_normalize(vec3 v)
{
    f32 len_sq = vec3_dot(v, v);
    if (len_sq <= 0.0f) {
        vec3 zero = {0.0f, 0.0f, 0.0f};
        return zero;
    }
    f32 inv_len = 1.0f / (f32)sqrt(len_sq);
    return vec3_scale(v, inv_len);
}
