#ifndef MATH_H
#define MATH_H

typedef float f32;
typedef double f64;

typedef struct {
    f32 x;
    f32 y;
} vec2;

typedef struct {
    f32 x;
    f32 y;
    f32 z;
} vec3;

vec2 vec2_add(vec2 a, vec2 b);
vec2 vec2_sub(vec2 a, vec2 b);
vec2 vec2_scale(vec2 v, f32 s);

vec3 vec3_add(vec3 a, vec3 b);
vec3 vec3_sub(vec3 a, vec3 b);
vec3 vec3_scale(vec3 v, f32 s);
f32 vec3_dot(vec3 a, vec3 b);
vec3 vec3_cross(vec3 a, vec3 b);
vec3 vec3_normalize(vec3 v);

#endif
