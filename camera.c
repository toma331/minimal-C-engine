#include <math.h>

#include "camera.h"

#define DEG_TO_RAD 0.017453292519943295f

static f32 clampf(f32 v, f32 lo, f32 hi)
{
    if (v < lo) {
        return lo;
    }
    if (v > hi) {
        return hi;
    }
    return v;
}

void camera_init(camera_t* cam)
{
    cam->position.x = 0.0f;
    cam->position.y = 0.0f;
    cam->position.z = 4.0f;
    cam->yaw_deg = -90.0f;
    cam->pitch_deg = 0.0f;
}

void camera_rotate(camera_t* cam, f32 delta_yaw_deg, f32 delta_pitch_deg)
{
    cam->yaw_deg += delta_yaw_deg;
    cam->pitch_deg += delta_pitch_deg;
    cam->pitch_deg = clampf(cam->pitch_deg, -89.0f, 89.0f);
}

void camera_zoom(camera_t* cam, f32 delta)
{
    vec3 forward;
    camera_get_forward(cam, &forward);
    cam->position = vec3_add(cam->position, vec3_scale(forward, delta));
}

void camera_get_position(const camera_t* cam, vec3* out_pos)
{
    *out_pos = cam->position;
}

void camera_get_forward(const camera_t* cam, vec3* out_forward)
{
    f32 yaw = cam->yaw_deg * DEG_TO_RAD;
    f32 pitch = cam->pitch_deg * DEG_TO_RAD;
    f32 cos_pitch = (f32)cos(pitch);

    out_forward->x = cos_pitch * (f32)cos(yaw);
    out_forward->y = (f32)sin(pitch);
    out_forward->z = cos_pitch * (f32)sin(yaw);
    *out_forward = vec3_normalize(*out_forward);
}

void camera_get_up(const camera_t* cam, vec3* out_up)
{
    (void)cam;
    out_up->x = 0.0f;
    out_up->y = 1.0f;
    out_up->z = 0.0f;
}
