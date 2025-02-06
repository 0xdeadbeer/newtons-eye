#ifndef IMGUI_HPP
#define IMGUI_HPP

#include <GLFW/glfw3.h>
#include <imgui.h>

void imgui_init(GLFWwindow *window);
void imgui_render(ImDrawData *draw_data);
void imgui_reset(int width, int height);
void imgui_events(float dt); 
void imgui_shutdown(void);
void imgui_set_clipboard_text(void *user_data, const char *text);
const char *imgui_get_clipboard_text(void *user_data);

#endif
