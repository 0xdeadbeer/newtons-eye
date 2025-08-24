#include "bgfx/embedded_shader.h"
#include "glm/ext/scalar_constants.hpp"
#include "imgui_internal.h"
#include <bgfx/defines.h>
#include <bx/file.h>
#include <bx/math.h>
#include <cmath>
#include <exception>
#include <glm/fwd.hpp>
#include <cstring>
#include <engine.hpp>
#include <bgfx/bgfx.h>
#include <glm/glm.hpp>
#include <common.hpp>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>
#include <engine/quad.hpp>
#include <imgui.h>
#include <engine/imgui.hpp>
#include <engine/graph.hpp>
#include <engine/model.hpp>

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
    this->input_map = 0; 
    this->debug_flag = 0; 
}

void Engine::keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
    engine->keyboard_slots[key] = action; 

    unsigned char new_input_map; 
    new_input_map |= (key == GLFW_KEY_A) && ((action & GLFW_REPEAT) || (action & GLFW_PRESS)) ? INPUT_A : 0;
    new_input_map |= (key == GLFW_KEY_D) && ((action & GLFW_REPEAT) || (action & GLFW_PRESS)) ? INPUT_D : 0; 
    new_input_map |= (key == GLFW_KEY_W) && ((action & GLFW_REPEAT) || (action & GLFW_PRESS)) ? INPUT_W : 0; 
    new_input_map |= (key == GLFW_KEY_S) && ((action & GLFW_REPEAT) || (action & GLFW_PRESS)) ? INPUT_S : 0; 
    new_input_map |= (key == GLFW_KEY_UP) && ((action & GLFW_REPEAT) || (action & GLFW_PRESS)) ? INPUT_ZOOM_IN : 0; 
    new_input_map |= (key == GLFW_KEY_DOWN) && ((action & GLFW_REPEAT) || (action & GLFW_PRESS)) ? INPUT_ZOOM_OUT : 0; 

    engine->input_map = new_input_map;
}

bx::Vec3 at = {0.0f, 0.0f, 0.0f};
bx::Vec3 eye = {0.0f, 0.0f, -35.0f};

double prev_x = 0.0f; 
void Engine::cursor_callback(GLFWwindow *window, double x, double y) {
    Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
    engine->cursor_xpos = x;
    engine->cursor_ypos = y;

    float dx = x-prev_x; 
    prev_x = x;
    eye.x -= dx / 10;
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
    engine->Reset();
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

void Engine::Reset(void) {
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

    glfwSetErrorCallback(this->error_callback);

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
    glfwSetKeyCallback(this->main_window, this->keyboard_callback);
    glfwSetCursorPosCallback(this->main_window, this->cursor_callback);
    glfwSetMouseButtonCallback(this->main_window, this->cursor_button_callback);
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
        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(this->width)/float(this->height), 0.1f, 500.0f, bgfx::getCaps()->homogeneousDepth);

        bgfx::setViewTransform(this->main_view, view, proj);
    }

    bgfx::setViewRect(this->main_view, 0, 0, this->width, this->height);

    this->u_position = bgfx::createUniform("u_position", bgfx::UniformType::Vec4);
    this->u_rotation = bgfx::createUniform("u_rotation", bgfx::UniformType::Vec4);
    this->u_scale = bgfx::createUniform("u_scale", bgfx::UniformType::Vec4);
    this->program = load_program(DEFAULT_VERTEX, DEFAULT_FRAGMENT);

    imgui_init(this->main_window);
    this->Reset();

    ret = this->UserLoad();
    if (ret < 0) {
        return -1;
    }

    return 0;
}

int computation_function_id = COMPUTATION_FUNCTION_WAVE; 
std::vector<std::string> computation_function_names = {"WAVE" ,"LINEAR XY", "TUBE", "BUMPS"};
float amplitude = 0.0f; 
float time_boost = 1.0f; 

float computation_function(float time, float x, float y) {
    switch (computation_function_id) {
        case COMPUTATION_FUNCTION_WAVE: 
            return amplitude * sqrt(x*x + y*y) + 
                (3 * cos(sqrt(x*x + y*y) - (time*time_boost)));
        case COMPUTATION_FUNCTION_XY:
            return amplitude*x*y;
        case COMPUTATION_FUNCTION_TUBE: 
            return 1/(amplitude * amplitude * (x*x + y*y));
        case COMPUTATION_FUNCTION_BUMPS: 
            return (sin(amplitude*x) * cos(amplitude*y))/amplitude;
    }
    return 0;
}

int Engine::UserLoad(void) {
    EngineObject obj; 
    obj.graph = new GraphComponent();
    obj.graph->calculate_callback = &computation_function;
    obj.graph->space_view = glm::vec3(10.0f);
    obj.graph->sampling_rate = 0.1f;

    obj.rotation.x = glm::pi<float>()/3;
    obj.position.z = 4.5f;

    this->objs.push_back(obj);

    return 0;
}

void Engine::ImguiUpdate(EngineObject *obj) {
    GraphComponent *graph = obj->graph;

    ImGui::ShowDemoWindow();

    ImGui::Begin("Multi-Dimensional Curve Renderer", NULL, ImGuiWindowFlags_NoResize);

    ImGui::Text("Written with love and passion by @0xdeadbeer");
    ImGui::Text("Libraries: BGFX, BX, BIMG, IMGUI, GLFW, ASSIMP, GLW");
    ImGui::Text("Source code publicly available online");


    // function select
    ImGui::SeparatorText("Functions");
    {
        int size = computation_function_names.size(); 
        for (int i = 0; i < size; i++) {
            ImGui::PushItemWidth(-100);
            if (ImGui::Selectable(computation_function_names.at(i).c_str(), computation_function_id == (1 << i))) {
                computation_function_id = 1 << i;
            }
        }

        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("amplitude", &amplitude, 0.01f, 0.0f, 15.0f);

        if (computation_function_id & COMPUTATION_FUNCTION_WAVE) {
            ImGui::PushItemWidth(-100);
            ImGui::DragFloat("time boost", &time_boost, 0.01f, 0.0f, 25.0f);
        }
    }

    // general properties
    ImGui::SeparatorText("General");
    {
        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("sampling rate", &graph->sampling_rate, 0.01f, 0.05f, 10.0f);

        if (ImGui::Button("Toggle Debug Wireframe")) {
            this->debug_flag = (this->debug_flag+1) % 2;
        }
    }

    ImGui::SeparatorText("Miniplots");
    {
        std::vector<float> xvalues; 
        for (float x = -graph->space_view.x; x < graph->space_view.x; x += graph->sampling_rate) {
           xvalues.push_back(-graph->calculate_callback(this->time, x, 0));
        }

        std::vector<float> yvalues;
        for (float y = -graph->space_view.y; y < graph->space_view.y; y += graph->sampling_rate) {
           yvalues.push_back(-graph->calculate_callback(this->time, 0, y));
        }

        ImGui::PlotLines("x/z", xvalues.data(), xvalues.size(), 0, NULL, -10.0f, 10.0f, ImVec2(0, 35.0f));
        ImGui::PlotLines("y/z", yvalues.data(), yvalues.size(), 0, NULL, -10.0f, 10.0f, ImVec2(0, 35.0f));
    }

    ImGui::SeparatorText("Space View");
    { 
        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("space view x", &graph->space_view.x, 0.01f, 1.0f, 50.0f);

        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("space view y", &graph->space_view.y, 0.01f, 1.0f, 50.0f);
    }

    ImGui::SeparatorText("Position");
    { 
        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("position x", &obj->position.x, 0.01f, -30.0f, 30.0f);

        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("position y", &obj->position.y, 0.01f, -30.0f, 30.0f);

        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("position z", &obj->position.z, 0.01f, -30.0f, 30.0f);
    }

    ImGui::SeparatorText("Rotation");
    { 
        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("rotation x", &obj->rotation.x, 0.01f, -2*glm::pi<float>(), 2*glm::pi<float>());

        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("rotation y", &obj->rotation.y, 0.01f, -2*glm::pi<float>(), 2*glm::pi<float>());

        ImGui::PushItemWidth(-100);
        ImGui::DragFloat("rotation z", &obj->rotation.z, 0.01f, -2*glm::pi<float>(), 2*glm::pi<float>());
    }

    ImGui::End();
}

void Engine::UserUpdate(void) {
    bgfx::setDebug(this->debug_flag ? BGFX_DEBUG_WIREFRAME : 0);


    EngineObject *obj = &(this->objs.at(0));
    GraphComponent *graph = obj->graph;

    std::vector<float> vertices; 
    std::vector<unsigned int> indices;

    this->ImguiUpdate(obj);

    int grid_width = 0; 
    int grid_height = 0; 
    int grid_verts = 0;
    for (float y = -graph->space_view.y; y < graph->space_view.y; y += graph->sampling_rate, grid_height++) {
        for (float x = -graph->space_view.x; x < graph->space_view.x; x += graph->sampling_rate) {
            graph->Calculate(x, y, vertices);
        }
    }

    grid_verts = vertices.size()/3;
    grid_width = grid_verts/grid_height;

    int grid_idx = 0; 
    for (int y = 0; y < grid_height-1; y++) {
        for (int x = 0; x < grid_width-1; x++) {
            int i = y*(grid_width) + x;
            indices.push_back(i);
            indices.push_back(i+1);
            indices.push_back(i+grid_width);

            indices.push_back(i+grid_width);
            indices.push_back(i+1+grid_width);
            indices.push_back(i+1);
        }
    }

    grid_idx = indices.size();

    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tvi;

    bgfx::allocTransientVertexBuffer(&tvb, grid_idx, graph->graph_layout);
    bgfx::allocTransientIndexBuffer(&tvi, grid_idx, true);

    // copy vertices 
    uint8_t *data = tvb.data;
    memcpy(data, vertices.data(), grid_verts*3*sizeof(float));

    // copy indices
    data = tvi.data;
    memcpy(data, indices.data(), grid_idx*sizeof(unsigned int));

    // render
    bgfx::setState(graph->render_state);
    bgfx::setVertexBuffer(0, &tvb);
    bgfx::setIndexBuffer(&tvi);
    bgfx::setUniform(this->u_position, &obj->position);
    bgfx::setUniform(this->u_rotation, &obj->rotation);
    bgfx::setUniform(this->u_scale, &obj->scale);
    bgfx::submit(this->main_view, this->program);
}

void Engine::Update(void) {
    this->time = glfwGetTime();
    this->dt = this->time - this->last_time;
    this->last_time = this->time;

    this->objs.at(0).graph->last_time = this->last_time;
    this->objs.at(0).graph->dt = this->dt;
    this->objs.at(0).graph->time = this->time;

    glfwPollEvents();

    imgui_events(this->dt);
    ImGui::NewFrame();
    bgfx::touch(this->main_view);

    this->UserUpdate();

    bgfx::frame();
    ImGui::Render();
    imgui_render(ImGui::GetDrawData());
}

void Engine::Shutdown(void) { 
    imgui_shutdown();

    bgfx::destroy(this->u_position);
    bgfx::destroy(this->u_rotation);
    bgfx::destroy(this->u_scale);

    bgfx::destroy(this->program);

    bgfx::shutdown();
    glfwTerminate();
}

void Engine::error_callback(int error, const char *s) {
    ERROR("glfw failed -> " << s);
}
