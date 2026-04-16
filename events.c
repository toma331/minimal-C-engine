#include <GLFW/glfw3.h>

#include "camera.h"
#include "events.h"

static camera_t* g_camera = 0;

static int key_w = 0;
static int key_a = 0;
static int key_s = 0;
static int key_d = 0;
static int key_up = 0;
static int key_down = 0;
static int first_mouse = 1;
static double last_x = 0.0;
static double last_y = 0.0;

void events_set_camera(camera_t* cam)
{
    g_camera = cam;
}

void events_center_mouse(GLFWwindow* window)
{
    (void)window;
    first_mouse = 1;
}

void events_on_mouse_button(GLFWwindow* window, int button, int action, int mods)
{
    (void)window;
    (void)button;
    (void)action;
    (void)mods;
}

void events_on_cursor_pos(GLFWwindow* window, double xpos, double ypos)
{
    (void)window;
    if (first_mouse) {
        last_x = xpos;
        last_y = ypos;
        first_mouse = 0;
        return;
    }

    if (g_camera) {
        const f32 mouse_sens = 0.05f;
        f32 x_offset = (f32)(xpos - last_x);
        f32 y_offset = (f32)(last_y - ypos);
        camera_rotate(g_camera, x_offset * mouse_sens, y_offset * mouse_sens);
    }

    last_x = xpos;
    last_y = ypos;
}

void events_on_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    int is_down = (action == GLFW_PRESS || action == GLFW_REPEAT);
    int is_up = (action == GLFW_RELEASE);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }

    if (key == GLFW_KEY_W) {
        key_w = is_down ? 1 : (is_up ? 0 : key_w);
    }
    if (key == GLFW_KEY_A) {
        key_a = is_down ? 1 : (is_up ? 0 : key_a);
    }
    if (key == GLFW_KEY_S) {
        key_s = is_down ? 1 : (is_up ? 0 : key_s);
    }
    if (key == GLFW_KEY_D) {
        key_d = is_down ? 1 : (is_up ? 0 : key_d);
    }
    if (key == GLFW_KEY_Q) {
        key_down = is_down ? 1 : (is_up ? 0 : key_down);
    }
    if (key == GLFW_KEY_E) {
        key_up = is_down ? 1 : (is_up ? 0 : key_up);
    }
}

void events_update(f32 dt)
{
    if (!g_camera)
        return;

    const f32 speed = 3.0f;
    vec3 forward;
    vec3 up;
    camera_get_forward(g_camera, &forward);
    camera_get_up(g_camera, &up);
    vec3 right = vec3_normalize(vec3_cross(forward, up));

    vec3 move = {0.0f, 0.0f, 0.0f};
    if (key_w)
        move = vec3_add(move, forward);
    if (key_s)
        move = vec3_sub(move, forward);
    if (key_d)
        move = vec3_add(move, right);
    if (key_a)
        move = vec3_sub(move, right);
    if (key_up)
        move = vec3_add(move, up);
    if (key_down)
        move = vec3_sub(move, up);

    if (move.x != 0.0f || move.y != 0.0f || move.z != 0.0f) {
        move = vec3_normalize(move);
        move = vec3_scale(move, speed * dt);
        g_camera->position = vec3_add(g_camera->position, move);
    }
}
