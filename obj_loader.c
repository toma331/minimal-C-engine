#include <GL/gl.h>

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "obj_loader.h"

static int grow_vec3_array(vec3** data, size_t* capacity, size_t needed)
{
    size_t new_capacity = *capacity;
    vec3* resized = NULL;

    if (needed <= *capacity) {
        return 0;
    }

    if (new_capacity == 0) {
        new_capacity = 256;
    }
    while (new_capacity < needed) {
        new_capacity *= 2;
    }

    resized = (vec3*)realloc(*data, new_capacity * sizeof(vec3));
    if (!resized) {
        return -1;
    }

    *data = resized;
    *capacity = new_capacity;
    return 0;
}

static int grow_uint_array(unsigned int** data, size_t* capacity, size_t needed)
{
    size_t new_capacity = *capacity;
    unsigned int* resized = NULL;

    if (needed <= *capacity) {
        return 0;
    }

    if (new_capacity == 0) {
        new_capacity = 512;
    }
    while (new_capacity < needed) {
        new_capacity *= 2;
    }

    resized = (unsigned int*)realloc(*data, new_capacity * sizeof(unsigned int));
    if (!resized) {
        return -1;
    }

    *data = resized;
    *capacity = new_capacity;
    return 0;
}

static int parse_positive_index(const char* token, unsigned int* out_index)
{
    char* end = NULL;
    long value = strtol(token, &end, 10);

    if (end == token || value <= 0 || (unsigned long)value > (unsigned long)UINT_MAX) {
        return -1;
    }

    *out_index = (unsigned int)(value - 1);
    return 0;
}

static int parse_face_vertex_token(const char* token,
                                   unsigned int* out_vertex_index,
                                   unsigned int* out_normal_index,
                                   int* out_has_normal)
{
    char buffer[64];
    size_t token_len = strlen(token);
    char* slash_1 = NULL;
    char* slash_2 = NULL;

    if (token_len == 0 || token_len >= sizeof(buffer)) {
        return -1;
    }

    memcpy(buffer, token, token_len + 1);

    slash_1 = strchr(buffer, '/');
    if (!slash_1) {
        *out_has_normal = 0;
        return parse_positive_index(buffer, out_vertex_index);
    }

    *slash_1 = '\0';
    if (parse_positive_index(buffer, out_vertex_index) != 0) {
        return -1;
    }

    slash_2 = strchr(slash_1 + 1, '/');
    if (!slash_2) {
        *out_has_normal = 0;
        return 0;
    }

    if (*(slash_2 + 1) == '\0') {
        *out_has_normal = 0;
        return 0;
    }

    if (parse_positive_index(slash_2 + 1, out_normal_index) != 0) {
        return -1;
    }

    *out_has_normal = 1;
    return 0;
}

static int build_smooth_normals(obj_mesh_t* mesh)
{
    size_t i;
    vec3* generated = NULL;

    if (!mesh || !mesh->vertices || !mesh->indices || mesh->vertex_count == 0 ||
        mesh->index_count < 3) {
        return -1;
    }

    generated = (vec3*)calloc(mesh->vertex_count, sizeof(vec3));
    if (!generated) {
        return -1;
    }

    for (i = 0; i + 2 < mesh->index_count; i += 3) {
        unsigned int i0 = mesh->indices[i + 0];
        unsigned int i1 = mesh->indices[i + 1];
        unsigned int i2 = mesh->indices[i + 2];
        vec3 v0;
        vec3 v1;
        vec3 v2;
        vec3 e1;
        vec3 e2;
        vec3 face_n;

        if (i0 >= mesh->vertex_count || i1 >= mesh->vertex_count || i2 >= mesh->vertex_count) {
            continue;
        }

        v0 = mesh->vertices[i0];
        v1 = mesh->vertices[i1];
        v2 = mesh->vertices[i2];
        e1 = vec3_sub(v1, v0);
        e2 = vec3_sub(v2, v0);
        face_n = vec3_cross(e1, e2);

        generated[i0] = vec3_add(generated[i0], face_n);
        generated[i1] = vec3_add(generated[i1], face_n);
        generated[i2] = vec3_add(generated[i2], face_n);
    }

    for (i = 0; i < mesh->vertex_count; ++i) {
        generated[i] = vec3_normalize(generated[i]);
    }

    free(mesh->normals);
    mesh->normals = generated;
    mesh->normal_count = mesh->vertex_count;

    for (i = 0; i < mesh->index_count; ++i) {
        mesh->normal_indices[i] = mesh->indices[i];
    }

    return 0;
}

int obj_load(const char* path, obj_mesh_t* out_mesh)
{
    FILE* file = NULL;
    char line[512];
    obj_mesh_t mesh = {0};
    size_t vertex_capacity = 0;
    size_t normal_capacity = 0;
    size_t index_capacity = 0;
    size_t normal_index_capacity = 0;
    int missing_normals = 0;
    int status = 0;

    if (!path || !out_mesh) {
        return -1;
    }

    file = fopen(path, "r");
    if (!file) {
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] == 'v' && isspace((unsigned char)line[1])) {
            vec3 v = {0.0f, 0.0f, 0.0f};
            if (sscanf(line + 1, "%f %f %f", &v.x, &v.y, &v.z) != 3) {
                continue;
            }
            if (grow_vec3_array(&mesh.vertices, &vertex_capacity, mesh.vertex_count + 1) != 0) {
                status = -1;
                break;
            }
            mesh.vertices[mesh.vertex_count++] = v;
            continue;
        }

        if (line[0] == 'v' && line[1] == 'n' && isspace((unsigned char)line[2])) {
            vec3 n = {0.0f, 0.0f, 0.0f};
            if (sscanf(line + 2, "%f %f %f", &n.x, &n.y, &n.z) != 3) {
                continue;
            }
            if (grow_vec3_array(&mesh.normals, &normal_capacity, mesh.normal_count + 1) != 0) {
                status = -1;
                break;
            }
            mesh.normals[mesh.normal_count++] = vec3_normalize(n);
            continue;
        }

        if (line[0] == 'f' && isspace((unsigned char)line[1])) {
            char t0[64];
            char t1[64];
            char t2[64];
            char t3[64];
            unsigned int v0 = 0;
            unsigned int v1 = 0;
            unsigned int v2 = 0;
            unsigned int n0 = 0;
            unsigned int n1 = 0;
            unsigned int n2 = 0;
            int has_n0 = 0;
            int has_n1 = 0;
            int has_n2 = 0;
            int tokens = sscanf(line + 1, "%63s %63s %63s %63s", t0, t1, t2, t3);

            if (tokens < 3) {
                continue;
            }
            if (tokens > 3) {
                status = -1;
                break;
            }

            if (parse_face_vertex_token(t0, &v0, &n0, &has_n0) != 0 ||
                parse_face_vertex_token(t1, &v1, &n1, &has_n1) != 0 ||
                parse_face_vertex_token(t2, &v2, &n2, &has_n2) != 0) {
                status = -1;
                break;
            }

            if (v0 >= mesh.vertex_count || v1 >= mesh.vertex_count || v2 >= mesh.vertex_count) {
                status = -1;
                break;
            }

            if ((has_n0 && n0 >= mesh.normal_count) || (has_n1 && n1 >= mesh.normal_count) ||
                (has_n2 && n2 >= mesh.normal_count)) {
                status = -1;
                break;
            }

            if (grow_uint_array(&mesh.indices, &index_capacity, mesh.index_count + 3) != 0 ||
                grow_uint_array(&mesh.normal_indices, &normal_index_capacity,
                                mesh.index_count + 3) != 0) {
                status = -1;
                break;
            }

            mesh.indices[mesh.index_count + 0] = v0;
            mesh.indices[mesh.index_count + 1] = v1;
            mesh.indices[mesh.index_count + 2] = v2;

            mesh.normal_indices[mesh.index_count + 0] = has_n0 ? n0 : UINT_MAX;
            mesh.normal_indices[mesh.index_count + 1] = has_n1 ? n1 : UINT_MAX;
            mesh.normal_indices[mesh.index_count + 2] = has_n2 ? n2 : UINT_MAX;

            if (!has_n0 || !has_n1 || !has_n2) {
                missing_normals = 1;
            }

            mesh.index_count += 3;
        }
    }

    if (file) {
        fclose(file);
        file = NULL;
    }

    if (status == 0 && (mesh.vertex_count == 0 || mesh.index_count == 0)) {
        status = -1;
    }

    if (status == 0 && (mesh.normal_count == 0 || missing_normals) &&
        build_smooth_normals(&mesh) != 0) {
        status = -1;
    }

    if (status != 0) {
        free(mesh.vertices);
        free(mesh.normals);
        free(mesh.indices);
        free(mesh.normal_indices);
        return -1;
    }

    *out_mesh = mesh;
    return 0;
}

void obj_free(obj_mesh_t* mesh)
{
    if (!mesh) {
        return;
    }
    free(mesh->vertices);
    free(mesh->normals);
    free(mesh->indices);
    free(mesh->normal_indices);
    mesh->vertices = NULL;
    mesh->normals = NULL;
    mesh->indices = NULL;
    mesh->normal_indices = NULL;
    mesh->vertex_count = 0;
    mesh->normal_count = 0;
    mesh->index_count = 0;
}

void obj_draw(const obj_mesh_t* mesh)
{
    size_t i;

    if (!mesh || !mesh->vertices || !mesh->normals || !mesh->indices || !mesh->normal_indices ||
        mesh->index_count < 3) {
        return;
    }

    glColor3f(0.85f, 0.85f, 0.88f);
    glBegin(GL_TRIANGLES);
    for (i = 0; i < mesh->index_count; ++i) {
        unsigned int vertex_idx = mesh->indices[i];
        unsigned int normal_idx = mesh->normal_indices[i];
        const vec3* v = NULL;
        const vec3* n = NULL;

        if (vertex_idx >= mesh->vertex_count || normal_idx >= mesh->normal_count) {
            continue;
        }

        v = &mesh->vertices[vertex_idx];
        n = &mesh->normals[normal_idx];

        glNormal3f(n->x, n->y, n->z);
        glVertex3f(v->x, v->y, v->z);
    }
    glEnd();
}
