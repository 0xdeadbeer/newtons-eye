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

int board_factor = 20.0f;

Engine::Engine(void) {
    this->width = DEFAULT_WIDTH;
    this->height = DEFAULT_HEIGHT;
    this->title = "Newton's Eye";

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

bx::Vec3 at = {0.0f, 0.0f, -1.0f};
bx::Vec3 eye = {0.0f, 0.0f, -35.0f};
bool mouseDown = false;

glm::vec4 *locked = NULL;

double prev_x = 0.0f; 
void Engine::cursor_callback(GLFWwindow *window, double x, double y) {
    Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
    engine->cursor_xpos = x;
    engine->cursor_ypos = y;

    if (!mouseDown) {
        locked = NULL; 
    }

    float dx = x-prev_x; 
    prev_x = x;
    eye.x -= dx / 10;

    double pos_x = (x/engine->width); 
    double pos_y = (engine->height-y)/engine->height;

    pos_x = pos_x * ((float) engine->width/engine->height) * 2 - ((float) engine->width/engine->height);
    pos_y = pos_y * 2 - 1;

    if (mouseDown && locked == NULL) {
        BoardComponent *board = engine->objs.at(0).board; 
        float dist1 = glm::distance(glm::vec2(board->first_body), glm::vec2(pos_x, pos_y));
        float dist2 = glm::distance(glm::vec2(board->second_body), glm::vec2(pos_x, pos_y));
        float dist3 = glm::distance(glm::vec2(board->third_body), glm::vec2(pos_x, pos_y));
        float dist4 = glm::distance(glm::vec2(board->fourth_body), glm::vec2(pos_x, pos_y));

        if (dist1 <= 0.1f)  {
            locked = &board->first_body; 
        }

        if (dist2 <= 0.1f)  {
            locked = &board->second_body; 
        }

        if (dist3 <= 0.1f)  {
            locked = &board->third_body; 
        }

        if (dist4 <= 0.1f)  {
            locked = &board->fourth_body; 
        }
    }

    if (locked != NULL) {
        locked->x = pos_x; 
        locked->y = pos_y; 
    }
}

void Engine::cursor_button_callback(GLFWwindow *window, int button, int action, int mods) {
    Engine *engine = (Engine *) glfwGetWindowUserPointer(window);
    engine->cursor_slots[button] = action;

    ImGuiIO &io = ImGui::GetIO();
    if (button >= 0 && button < IM_ARRAYSIZE(io.MouseDown)) {
        if (action == GLFW_PRESS) {
            io.MouseDown[button] = true;
            mouseDown = true;
        }
        else {
            io.MouseDown[button] = false;
            mouseDown = false;
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
            0x44444400,
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

        bx::mtxOrtho(proj, -this->width/20, this->width/20, -this->height/20, this->height/20, 0.01f, 100.0f, 0.0f, false);

        bgfx::setViewTransform(this->main_view, view, proj);
    }

    bgfx::setViewRect(this->main_view, 0, 0, this->width, this->height);

    this->u_mass = bgfx::createUniform("u_mass", bgfx::UniformType::Mat4);
    this->u_initialSpeed= bgfx::createUniform("u_initialSpeed", bgfx::UniformType::Vec4);
    this->u_firstBody = bgfx::createUniform("u_firstBody", bgfx::UniformType::Vec4);
    this->u_secondBody = bgfx::createUniform("u_secondBody", bgfx::UniformType::Vec4);
    this->u_thirdBody = bgfx::createUniform("u_thirdBody", bgfx::UniformType::Vec4);
    this->u_fourthBody= bgfx::createUniform("u_fourthBody", bgfx::UniformType::Vec4);
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
    obj.board = new BoardComponent(
        glm::vec4(-0.5, 0.0f, 0.0f, 0.0f), 
        glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
        glm::vec4(0.0f, -0.5f, 0.0f, 0.0f)
    );

    obj.scale.x = board_factor;
    obj.scale.y = board_factor;

    this->objs.push_back(obj);

    return 0;
}

void Engine::ImguiUpdate(EngineObject *obj) {
    ImGui::Begin("Gravity drop-test visualizer", NULL, 0);

    ImGui::Text("Written with love and passion by @0xdeadbeer");
    ImGui::Text("Libraries: BGFX, BX, BIMG, IMGUI, GLFW, ASSIMP, GLW");
    ImGui::Text("Source code publicly available online");

    {
        ImGui::SliderFloat("initial particle speed x", &(obj->board->initial_speed.x), 0, 1);
        ImGui::SliderFloat("initial particle speed y", &(obj->board->initial_speed.y), 0, 1);

        ImGui::Separator();

        ImGui::SliderFloat("first body mass decimal", &(obj->board->masses[0].x), -9, 9);
        ImGui::SliderFloat("first body mass e", &(obj->board->masses[0].y), 0, 50);

        ImGui::Separator();

        ImGui::SliderFloat("second body mass decimal", &(obj->board->masses[1].x), -9, 9);
        ImGui::SliderFloat("second body mass e", &(obj->board->masses[1].y), 0, 50);

        ImGui::Separator();

        ImGui::SliderFloat("third body mass decimal", &(obj->board->masses[2].x), -9, 9);
        ImGui::SliderFloat("third body mass e", &(obj->board->masses[2].y), 0, 50);

        ImGui::Separator();

        ImGui::SliderFloat("fourth body mass decimal", &(obj->board->masses[3].x), -9, 9);
        ImGui::SliderFloat("fourth body mass e", &(obj->board->masses[3].y), 0, 50);
    }

    {
        ImGui::Separator();

        bool changed = ImGui::SliderInt("Scaling factor", &board_factor, 20, 100);
        if (changed) {
            this->objs.at(0).scale.x = board_factor;
            this->objs.at(0).scale.y = board_factor;
        }
    }

    ImGui::End();
}

void Engine::UserUpdate(void) {
    bgfx::setDebug(this->debug_flag ? BGFX_DEBUG_WIREFRAME : 0);
    // bgfx::setDebug(BGFX_DEBUG_STATS);

    EngineObject *obj = &(this->objs.at(0));
    BoardComponent *board = obj->board;

    std::vector<float> vertices; 
    std::vector<unsigned int> indices;

    this->ImguiUpdate(obj);

    vertices.push_back((float)-this->width/this->height);
    vertices.push_back(-1.0f);
    vertices.push_back(0.0f);

    vertices.push_back((float)this->width/this->height);
    vertices.push_back(-1.0f);
    vertices.push_back(0.0f);

    vertices.push_back((float)this->width/this->height);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f);

    vertices.push_back((float)-this->width/this->height);
    vertices.push_back(1.0f);
    vertices.push_back(0.0f);

    int grid_verts = vertices.size()/3;
    int grid_idx = 0; 

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(3);

    indices.push_back(3);
    indices.push_back(2);
    indices.push_back(1);

    grid_idx = indices.size();

    bgfx::TransientVertexBuffer tvb;
    bgfx::TransientIndexBuffer tvi;

    bgfx::allocTransientVertexBuffer(&tvb, grid_idx, board->layout);
    bgfx::allocTransientIndexBuffer(&tvi, grid_idx, true);

    // copy vertices 
    uint8_t *data = tvb.data;
    memcpy(data, vertices.data(), grid_verts*3*sizeof(float));

    // copy indices
    data = tvi.data;
    memcpy(data, indices.data(), grid_idx*sizeof(unsigned int));

    // render
    bgfx::setState(board->render_state);
    bgfx::setVertexBuffer(0, &tvb);
    bgfx::setIndexBuffer(&tvi);
    bgfx::setUniform(this->u_mass, &obj->board->masses);
    bgfx::setUniform(this->u_initialSpeed, &obj->board->initial_speed);
    bgfx::setUniform(this->u_firstBody, &obj->board->first_body);
    bgfx::setUniform(this->u_secondBody, &obj->board->second_body);
    bgfx::setUniform(this->u_thirdBody, &obj->board->third_body);
    bgfx::setUniform(this->u_fourthBody, &obj->board->fourth_body);
    bgfx::setUniform(this->u_position, &obj->position);
    bgfx::setUniform(this->u_rotation, &obj->rotation);
    bgfx::setUniform(this->u_scale, &obj->scale);
    bgfx::submit(this->main_view, this->program);
}

void Engine::Update(void) {
    this->time = glfwGetTime();
    this->dt = this->time - this->last_time;
    this->last_time = this->time;

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
