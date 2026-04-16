#include <GL/gl.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <time.h>

#define WIGHT 1920
#define HEIGHT 1080

#include "render.h"
#include "events.h"
#include "camera.h"

static draw_callback g_draw_callback = 0;
static f32 g_delta_time = 0.0f;
static double g_last_time = 0.0;
static int g_fps_cap = 60;
static double g_target_frame_s = 1.0 / 60.0;
static camera_t g_camera;
static GLFWwindow* g_window = NULL;
static int g_width = WIGHT;
static int g_height = HEIGHT;

static void sleep_for(double seconds)
{
    if (seconds <= 0.0) {
        return;
    }
    struct timespec ts;
    ts.tv_sec = (time_t)seconds;
    ts.tv_nsec = (long)((seconds - (double)ts.tv_sec) * 1000000000.0);
    if (ts.tv_nsec < 0) {
        ts.tv_nsec = 0;
    }
    nanosleep(&ts, NULL);
}

static void set_projection(void)
{
    vec3 cam_pos;
    vec3 cam_up;
    vec3 cam_forward;
    vec3 cam_center;
    f64 aspect = 1.0;

    camera_get_position(&g_camera, &cam_pos);
    camera_get_up(&g_camera, &cam_up);
    camera_get_forward(&g_camera, &cam_forward);
    cam_center = vec3_add(cam_pos, cam_forward);

    if (g_height > 0) {
        aspect = (f64)g_width / (f64)g_height;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, aspect, 0.1, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cam_pos.x, cam_pos.y, cam_pos.z,
              cam_center.x, cam_center.y, cam_center.z,
              cam_up.x, cam_up.y, cam_up.z);
}

static void framebuffer_size(GLFWwindow* window, int w, int h)
{
    (void)window;
    if (w < 1) {
        w = 1;
    }
    if (h < 1) {
        h = 1;
    }
    g_width = w;
    g_height = h;
    glViewport(0, 0, g_width, g_height);
}

static void add_line(float ox, float x0, float y0, float x1, float y1)
{
    glVertex3f(ox + x0, y0, 0.0f);
    glVertex3f(ox + x1, y1, 0.0f);
}

enum {
    SEG_A = 1 << 0,
    SEG_B = 1 << 1,
    SEG_C = 1 << 2,
    SEG_D = 1 << 3,
    SEG_E = 1 << 4,
    SEG_F = 1 << 5,
    SEG_G = 1 << 6
};

static void draw_seven_seg(int mask, float ox)
{
    if (mask & SEG_A)
        add_line(ox, 0.0f, 1.0f, 1.0f, 1.0f);
    if (mask & SEG_B)
        add_line(ox, 1.0f, 1.0f, 1.0f, 0.5f);
    if (mask & SEG_C)
        add_line(ox, 1.0f, 0.5f, 1.0f, 0.0f);
    if (mask & SEG_D)
        add_line(ox, 0.0f, 0.0f, 1.0f, 0.0f);
    if (mask & SEG_E)
        add_line(ox, 0.0f, 0.0f, 0.0f, 0.5f);
    if (mask & SEG_F)
        add_line(ox, 0.0f, 0.5f, 0.0f, 1.0f);
    if (mask & SEG_G)
        add_line(ox, 0.0f, 0.5f, 1.0f, 0.5f);
}

static float draw_glyph(char c, float ox)
{
    switch (c) {
        case '0':
            draw_seven_seg(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F, ox);
            return 1.2f;
        case '1':
            draw_seven_seg(SEG_B | SEG_C, ox);
            return 1.2f;
        case '2':
            draw_seven_seg(SEG_A | SEG_B | SEG_G | SEG_E | SEG_D, ox);
            return 1.2f;
        case '3':
            draw_seven_seg(SEG_A | SEG_B | SEG_G | SEG_C | SEG_D, ox);
            return 1.2f;
        case '4':
            draw_seven_seg(SEG_F | SEG_G | SEG_B | SEG_C, ox);
            return 1.2f;
        case '5':
            draw_seven_seg(SEG_A | SEG_F | SEG_G | SEG_C | SEG_D, ox);
            return 1.2f;
        case '6':
            draw_seven_seg(SEG_A | SEG_F | SEG_G | SEG_E | SEG_C | SEG_D, ox);
            return 1.2f;
        case '7':
            draw_seven_seg(SEG_A | SEG_B | SEG_C, ox);
            return 1.2f;
        case '8':
            draw_seven_seg(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G, ox);
            return 1.2f;
        case '9':
            draw_seven_seg(SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G, ox);
            return 1.2f;
        case 'F':
            draw_seven_seg(SEG_A | SEG_F | SEG_G | SEG_E, ox);
            return 1.2f;
        case 'P':
            draw_seven_seg(SEG_A | SEG_F | SEG_G | SEG_B | SEG_E, ox);
            return 1.2f;
        case 'S':
            draw_seven_seg(SEG_A | SEG_F | SEG_G | SEG_C | SEG_D, ox);
            return 1.2f;
        case ':':
            add_line(ox, 0.5f, 0.75f, 0.5f, 0.85f);
            add_line(ox, 0.5f, 0.15f, 0.5f, 0.25f);
            return 0.6f;
        case 'X':
            add_line(ox, 0.0f, 0.0f, 1.0f, 1.0f);
            add_line(ox, 0.0f, 1.0f, 1.0f, 0.0f);
            return 1.2f;
        case 'Y':
            add_line(ox, 0.0f, 1.0f, 0.5f, 0.5f);
            add_line(ox, 1.0f, 1.0f, 0.5f, 0.5f);
            add_line(ox, 0.5f, 0.5f, 0.5f, 0.0f);
            return 1.2f;
        case 'Z':
            add_line(ox, 0.0f, 1.0f, 1.0f, 1.0f);
            add_line(ox, 1.0f, 1.0f, 0.0f, 0.0f);
            add_line(ox, 0.0f, 0.0f, 1.0f, 0.0f);
            return 1.2f;
        case ' ':
            return 0.6f;
        default:
            return 1.2f;
    }
}

void render_text(const char* text, const vec3* pos, f32 scale)
{
    if (!text || !pos) {
        return;
    }

    glPushMatrix();
    glTranslatef(pos->x, pos->y, pos->z);
    glScalef(scale, scale, scale);

    glBegin(GL_LINES);
    float x = 0.0f;
    for (const char* p = text; *p != '\0'; ++p) {
        x += draw_glyph(*p, x);
    }
    glEnd();

    glPopMatrix();
}

void init(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    if (!glfwInit()) {
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);

    g_window = glfwCreateWindow(WIGHT, HEIGHT, "minimal engine", NULL, NULL);
    if (!g_window) {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(g_window);
    glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(g_window, framebuffer_size);
    glfwSetKeyCallback(g_window, events_on_key);
    glfwSetCursorPosCallback(g_window, events_on_cursor_pos);
    glfwSetMouseButtonCallback(g_window, events_on_mouse_button);

    glfwGetFramebufferSize(g_window, &g_width, &g_height);
    glViewport(0, 0, g_width, g_height);

    camera_init(&g_camera);
    events_set_camera(&g_camera);

    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    events_center_mouse(g_window);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    {
        const GLfloat light_ambient[] = {0.18f, 0.18f, 0.2f, 1.0f};
        const GLfloat light_diffuse[] = {0.95f, 0.95f, 0.95f, 1.0f};
        const GLfloat light_specular[] = {0.35f, 0.35f, 0.35f, 1.0f};
        const GLfloat mat_specular[] = {0.25f, 0.25f, 0.25f, 1.0f};

        glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
        glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24.0f);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void set_draw_callback(draw_callback callback)
{
    g_draw_callback = callback;
}

void set_fps_cap(int fps)
{
    if (fps < 1) {
        fps = 1;
    }
    g_fps_cap = fps;
    g_target_frame_s = 1.0 / (double)g_fps_cap;
    if (g_target_frame_s <= 0.0) {
        g_target_frame_s = 0.000001;
    }
}

f32 get_delta_time(void)
{
    return g_delta_time;
}

int render_get_width(void)
{
    return g_width;
}

int render_get_height(void)
{
    return g_height;
}

void render_get_camera_position(vec3* out_pos)
{
    if (!out_pos) {
        return;
    }
    camera_get_position(&g_camera, out_pos);
}

void run(void)
{
    g_last_time = glfwGetTime();

    while (!glfwWindowShouldClose(g_window)) {
        glfwPollEvents();

        double now = glfwGetTime();
        double frame_time = now - g_last_time;
        if (frame_time < 0.0) {
            frame_time = 0.0;
        }
        g_last_time = now;
        g_delta_time = (f32)frame_time;

        events_update(g_delta_time);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        set_projection();

        if (g_draw_callback) {
            g_draw_callback();
        }

        glfwSwapBuffers(g_window);

        if (g_fps_cap > 0) {
            double after = glfwGetTime();
            double elapsed = after - now;
            double remaining = g_target_frame_s - elapsed;
            if (remaining > 0.0) {
                sleep_for(remaining);
            }
        }
    }

    glfwDestroyWindow(g_window);
    g_window = NULL;
    glfwTerminate();
}
