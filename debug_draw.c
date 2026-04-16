#include <GL/gl.h>
#include <GL/glu.h>

#include <math.h>
#include <stdio.h>

#include "debug_draw.h"
#include "render.h"

void debug_draw_vec3(const vec3* vector, const vec3* pos)
{
    glBegin(GL_LINES);
    glColor3f(1, 1, 0);
    glVertex3f(pos->x, pos->y, pos->z);
    glVertex3f(vector->x, vector->y, vector->z);
    glEnd();
}

void debug_draw_vec2(const vec2* vector, const vec2* pos)
{
    glBegin(GL_LINES);
    glColor3f(0, 1, 1);
    glVertex2f(pos->x, pos->y);
    glVertex2f(vector->x, vector->y);
    glEnd();
}

void debug_draw_axes(void)
{
    GLboolean depth_enabled = glIsEnabled(GL_DEPTH_TEST);
    if (depth_enabled) {
        glDisable(GL_DEPTH_TEST);
    }

    glLineWidth(2.0f);

    glBegin(GL_LINES);
    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1.5f, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1.5f, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1.5f);
    glEnd();

    {
        vec3 x_label = {1.6f, 0.0f, 0.0f};
        vec3 y_label = {0.0f, 1.6f, 0.0f};
        vec3 z_label = {0.0f, 0.0f, 1.6f};
        const f32 label_scale = 0.05f;

        glColor3f(1, 0, 0);
        render_text("X", &x_label, label_scale);
        glColor3f(0, 1, 0);
        render_text("Y", &y_label, label_scale);
        glColor3f(0, 0, 1);
        render_text("Z", &z_label, label_scale);
    }

    glLineWidth(1.0f);
    if (depth_enabled) {
        glEnable(GL_DEPTH_TEST);
    }
}

void debug_draw_infinite_grid(const vec3* center, f32 cell_size, int half_cells, f32 y, int major_every)
{
    if (!center) {
        return;
    }
    if (cell_size <= 0.0f || half_cells < 1) {
        return;
    }

    int base_x = (int)floorf(center->x / cell_size);
    int base_z = (int)floorf(center->z / cell_size);
    int start_x = base_x - half_cells;
    int start_z = base_z - half_cells;
    int end_x = start_x + half_cells * 2;
    int end_z = start_z + half_cells * 2;
    int lines = half_cells * 2 + 1;

    glBegin(GL_LINES);

    for (int i = 0; i < lines; ++i) {
        int grid_x = start_x + i;
        float x = (f32)grid_x * cell_size;

        if (major_every > 0 && (grid_x % major_every) == 0) {
            glColor3f(0.35f, 0.35f, 0.35f);
        } else {
            glColor3f(0.2f, 0.2f, 0.2f);
        }

        glVertex3f(x, y, (f32)start_z * cell_size);
        glVertex3f(x, y, (f32)end_z * cell_size);
    }

    for (int i = 0; i < lines; ++i) {
        int grid_z = start_z + i;
        float z = (f32)grid_z * cell_size;

        if (major_every > 0 && (grid_z % major_every) == 0) {
            glColor3f(0.35f, 0.35f, 0.35f);
        } else {
            glColor3f(0.2f, 0.2f, 0.2f);
        }

        glVertex3f((f32)start_x * cell_size, y, z);
        glVertex3f((f32)end_x * cell_size, y, z);
    }

    glEnd();
}

void debug_draw_cube(const f32* vertices)
{
    glColor3f(1.0, 1.0, 1.0);

    for (int i = 0; i < 6; i++) {
        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < 4; j++) {

            int idx = i * 12 + j * 3;
            glVertex3f(
                vertices[idx + 0],
                vertices[idx + 1],
                vertices[idx + 2]);
        }
        glEnd();
    }

    glFlush();
}

void debug_draw_colored_cube(const f32* vertices)
{
    glColor3f(1.0, 1.0, 1.0);

    for (int i = 0; i < 6; i++) {
        glBegin(GL_POLYGON);
        for (int j = 0; j < 4; j++) {

            int idx = i * 12 + j * 3;
            glVertex3f(
                vertices[idx + 0],
                vertices[idx + 1],
                vertices[idx + 2]);
        }
        glEnd();
    }

    glFlush();
}

void debug_draw_fps(void)
{
    static f32 acc = 0.0f;
    static int frames = 0;
    static int fps = 0;
    char text[32];
    int w = render_get_width();
    int h = render_get_height();
    vec3 pos = {10.0f, (f32)h - 22.0f, 0.0f};

    f32 dt = get_delta_time();
    if (dt < 0.0f) {
        dt = 0.0f;
    }
    acc += dt;
    frames += 1;
    if (acc >= 1.0f) {
        fps = (int)((f32)frames / acc + 0.5f);
        acc = 0.0f;
        frames = 0;
    }

    snprintf(text, sizeof(text), "FPS: %d", fps);

    GLboolean depth_enabled = glIsEnabled(GL_DEPTH_TEST);
    if (depth_enabled) {
        glDisable(GL_DEPTH_TEST);
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, (f64)w, 0.0, (f64)h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 1.0f);
    render_text(text, &pos, 12.0f);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    if (depth_enabled) {
        glEnable(GL_DEPTH_TEST);
    }
}
