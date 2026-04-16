#ifndef RENDER_H
#define RENDER_H

#include "math.h"

void init(int argc, char** argv);
typedef void (*draw_callback)(void);
void set_draw_callback(draw_callback callback);
void run(void);
void render_text(const char* text, const vec3* pos, f32 scale);
void set_fps_cap(int fps);
f32 get_delta_time(void);
int render_get_width(void);
int render_get_height(void);
void render_get_camera_position(vec3* out_pos);

#endif
