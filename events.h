#ifndef EVENTS_H
#define EVENTS_H

#include "camera.h"

struct GLFWwindow;

void events_set_camera(camera_t* cam);
void events_center_mouse(struct GLFWwindow* window);
void events_on_mouse_button(struct GLFWwindow* window, int button, int action, int mods);
void events_on_cursor_pos(struct GLFWwindow* window, double xpos, double ypos);
void events_on_key(struct GLFWwindow* window, int key, int scancode, int action, int mods);
void events_update(f32 dt);

#endif
