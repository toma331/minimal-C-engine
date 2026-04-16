#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include <stddef.h>

#include "math.h"

typedef struct {
    vec3* vertices;
    vec3* normals;
    unsigned int* indices;
    unsigned int* normal_indices;
    size_t vertex_count;
    size_t normal_count;
    size_t index_count;
} obj_mesh_t;

int obj_load(const char* path, obj_mesh_t* out_mesh);
void obj_free(obj_mesh_t* mesh);
void obj_draw(const obj_mesh_t* mesh);

#endif
