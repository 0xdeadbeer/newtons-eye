#include <iostream>
#include "bgfx/defines.h"
#include "engine/object.hpp"
#include "imgui.h"
#include <bx/file.h>
#include <bx/math.h>
#include <glm/fwd.hpp>
#include <vector>
#include <cstring>
#include <engine.hpp>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <common.hpp>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <engine/quad.hpp>
#include <engine/imgui.hpp>

Engine::Engine(void) {
    this->width = DEFAULT_WIDTH;
    this->height = DEFAULT_HEIGHT;
    this->title = "Polynomial Graphical Viewer";

    this->main_view = 0;

    memset(this->keyboard_slots, 0, sizeof(this->keyboard_slots));
    memset(this->cursor_slots, 0, sizeof(this->cursor_slots));

    this->last_time = 0; 
    this->dt = 0; 
    this->time = 0; 
}

void Engine::keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
    engine->keyboard_slots[key] = action; 
}

void Engine::cursor_callback(GLFWwindow *window, double x, double y) {
    Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
    engine->cursor_xpos = x;
    engine->cursor_ypos = y;
}

void Engine::cursor_button_callback(GLFWwindow *window, int button, int action, int mods) {
    Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
    engine->cursor_slots[button] = action;

    ImGuiIO &io = ImGui::GetIO();
    if (button >= 0 && button < IM_ARRAYSIZE(io.MouseDown)) {
        if (action == GLFW_PRESS) {
            io.MouseDown[button] = true;
        }
        else {
            io.MouseDown[button] = false;
        }
    }
}

void Engine::window_size_callback(GLFWwindow *window, int width, int height) {
    Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
    engine->width = width; 
    engine->height = height;
    engine->reset();
}
void Engine::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    ImGuiIO &io = ImGui::GetIO();
    io.MouseWheelH += xoffset;
    io.MouseWheel += yoffset;
}

void Engine::char_callback(GLFWwindow *window, unsigned int codepoint) {
    ImGuiIO &io = ImGui::GetIO();
    io.AddInputCharacter(codepoint);
}

void Engine::reset(void) {
    bgfx::reset(this->width, this->height, 0);
    imgui_reset(this->width, this->height);
    bgfx::setViewRect(this->main_view, 0, 0, width, height);
    bgfx::setViewClear(this->main_view, 
            BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 
            0x00000000,
            1.0f,
            0);
}

int Engine::Init(void) {
    int ret = glfwInit();
    if (ret < 0) {
        ERROR("failed initializing glfw");
        return -1;
    }

    glfwSetErrorCallback(Engine::GlfwErrorCallback);

    this->main_window = glfwCreateWindow(this->width, this->height, this->title.c_str(), 
#ifdef GLFW_DEBUG
            NULL
#else 
            glfwGetPrimaryMonitor() // fullscreen
#endif
            , NULL);
    if (!this->main_window) {
        ERROR("failed creating window");
        return -1;
    }

    glfwSetWindowUserPointer(this->main_window, this);
    glfwSetKeyCallback(this->main_window, Engine::keyboard_callback);
    glfwSetCursorPosCallback(this->main_window, Engine::cursor_callback);
    glfwSetMouseButtonCallback(this->main_window, Engine::cursor_button_callback);
    glfwSetWindowSizeCallback(this->main_window, this->window_size_callback);
    glfwSetScrollCallback(this->main_window, this->scroll_callback);
    glfwSetCharCallback(this->main_window, this->char_callback);;

#ifdef GLFW_DEBUG
    glfwSetInputMode(this->main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif

    bgfx::Init init;
    init.platformData.ndt = glfwGetX11Display();
    init.platformData.nwh = (void*) (uintptr_t) glfwGetX11Window(this->main_window);
    
    glfwGetWindowSize(this->main_window, &this->width, &this->height);
    init.resolution.width = this->width; 
    init.resolution.height = this->height; 
    init.resolution.reset = BGFX_RESET_VSYNC;
    if (!bgfx::init(init)) {
        ERROR("failed initializing bgfx");
        return -1;
    }

    bgfx::setViewClear(this->main_view, 
            BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 
            0x00000000,
            1.0f,
            0);

    {
        bx::Vec3 at = {0.0f, 0.0f, 0.0f};
        bx::Vec3 eye = {0.0f, 0.0f, -35.0f};

        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(this->width)/float(this->height), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

        bgfx::setViewTransform(this->main_view, view, proj);
    }

    bgfx::setViewRect(this->main_view, 0, 0, this->width, this->height);

    this->u_position = bgfx::createUniform("u_position", bgfx::UniformType::Vec4);
    this->u_rotation = bgfx::createUniform("u_rotation", bgfx::UniformType::Vec4);
    this->u_scale = bgfx::createUniform("u_scale", bgfx::UniformType::Vec4);
    this->program = load_program(DEFAULT_VERTEX, DEFAULT_FRAGMENT);

    imgui_init(this->main_window);
    reset();

    return 0;
}

float angular_velocity[3];

int Engine::Update(void) {
    this->time = glfwGetTime();
    this->dt = this->time - this->last_time;
    this->last_time = this->time;

    glfwPollEvents();

    imgui_events(this->dt);

    ImGui::NewFrame();

    ///////////////
    // UPDATE 
    ///////////////

    this->objs.at(0)->rotation.x += angular_velocity[0] * dt;  
    this->objs.at(0)->rotation.y += angular_velocity[1] * dt;  
    this->objs.at(0)->rotation.z += angular_velocity[2] * dt;  

    bgfx::touch(this->main_view);
    ImGui::ShowDemoWindow();

    ImGui::Begin("Hello there");
    ImGui::Text("Hello theree!");
    ImGui::SliderFloat("angular velocity (x component)", &angular_velocity[0], -2.5f, 2.5f);
    ImGui::SliderFloat("angular velocity (y component)", &angular_velocity[1], -2.5f, 2.5f);
    ImGui::SliderFloat("angular velocity (z component)", &angular_velocity[2], -2.5f, 2.5f);
    ImGui::End();
    
    ///////////////
    // UPDATE 
    ///////////////

    ImGui::Render();
    imgui_render(ImGui::GetDrawData());

    std::int32_t new_width;
    std::int32_t new_height;
    glfwGetFramebufferSize(this->main_window, &new_width, &new_height);
    if (new_width != this->width || new_height != this->height) {
        bgfx::reset(new_width, new_height, 0);
        bgfx::setViewRect(this->main_view, 0, 0, bgfx::BackbufferRatio::Equal);
        this->width = new_width;
        this->height = new_height;
    }


    for (int i = 0; i < this->objs.size(); i++) {
        bgfx::setState(BGFX_STATE_WRITE_R |
                BGFX_STATE_WRITE_G | 
                BGFX_STATE_WRITE_B | 
                BGFX_STATE_WRITE_A | 
                BGFX_STATE_WRITE_Z |
                BGFX_STATE_DEPTH_TEST_LESS | 
                BGFX_STATE_CULL_CCW );

        EngineObject* obj = this->objs[i];

        bgfx::setVertexBuffer(0, obj->vbh);
        bgfx::setIndexBuffer(obj->ibh);

        bgfx::setUniform(this->u_position, &obj->position);
        bgfx::setUniform(this->u_rotation, &obj->rotation);
        bgfx::setUniform(this->u_scale, &obj->scale);

        bgfx::submit(this->main_view, this->program);
    }

    bgfx::frame();

    return 0;
}

void Engine::Shutdown(void) { 
    imgui_shutdown();

    for (int i = 0; i < this->objs.size(); i++) {
        EngineObject* obj = this->objs[i];
        bgfx::destroy(obj->vbh);
        bgfx::destroy(obj->ibh);
    }

    bgfx::destroy(this->u_position);
    bgfx::destroy(this->u_rotation);
    bgfx::destroy(this->u_scale);

    bgfx::destroy(this->program);

    bgfx::shutdown();
    glfwTerminate();
}

void Engine::GlfwErrorCallback(int error, const char *s) {
    ERROR("glfw failed -> " << s);
}

void Engine::Instantiate(EngineObject* obj) {
    this->objs.push_back(obj);
}
