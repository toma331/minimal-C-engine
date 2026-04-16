#include <GL/gl.h>
#include <stdio.h>

#include "debug_draw.h"
#include "obj_loader.h"
#include "render.h"
#include "math.h"

static obj_mesh_t g_suzanne;

static void draw(void)
{
    static f32 angle_deg = 0.0f;
    f32 dt = get_delta_time();
    vec3 cam_pos;
    const GLfloat light_pos[] = {3.0f, 4.5f, 3.0f, 1.0f};

    angle_deg += 90.0f * dt;
    if (angle_deg >= 360.0f) {
        angle_deg -= 360.0f;
    }

    glDisable(GL_LIGHTING);
    debug_draw_fps();
    // debug_draw_axes();
    render_get_camera_position(&cam_pos);
    debug_draw_infinite_grid(&cam_pos, 1.0f, 50, 0.0f, 5);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    glPushMatrix();
    glRotatef(angle_deg, 0.0f, 1.0f, 0.0f);
    glRotatef(angle_deg * 0.5f, 1.0f, 0.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);
    obj_draw(&g_suzanne);
    glPopMatrix();
}

int main(int argc, char** argv)
{
    if (obj_load("obj/suzanne.obj", &g_suzanne) != 0) {
        fprintf(stderr, "failed to load obj/suzanne.obj\n");
        return 1;
    }

    init(argc, argv);

    set_draw_callback(draw);

    run();
    obj_free(&g_suzanne);

    return 0;
}
