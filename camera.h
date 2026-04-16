#ifndef CAMERA_H
#define CAMERA_H

#include "math.h"

typedef struct {
    vec3 position;
    f32 yaw_deg;
    f32 pitch_deg;
} camera_t;

void camera_init(camera_t* cam);
void camera_rotate(camera_t* cam, f32 delta_yaw_deg, f32 delta_pitch_deg);
void camera_zoom(camera_t* cam, f32 delta);
void camera_get_position(const camera_t* cam, vec3* out_pos);
void camera_get_forward(const camera_t* cam, vec3* out_forward);
void camera_get_up(const camera_t* cam, vec3* out_up);

#endif
