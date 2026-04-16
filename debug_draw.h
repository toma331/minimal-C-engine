#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include "math.h"

void debug_draw_axes(void);
void debug_draw_vec3(const vec3* vector, const vec3* pos);
void debug_draw_vec2(const vec2* vector, const vec2* pos);
void debug_draw_cube(const f32* vertices);
void debug_draw_colored_cube(const f32* vertices);
void debug_draw_fps(void);
void debug_draw_infinite_grid(const vec3* center, f32 cell_size, int half_cells, f32 y, int major_every);

#endif
