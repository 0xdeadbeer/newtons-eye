#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <iostream>
#include <bx/file.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <engine/object.hpp>

#define CAMERA_WIDTH 50.0f
#define CAMERA_NEAR 0.01f
#define CAMERA_FAR 100.0f

class Engine {
    public:
        Engine(void);
        int Init(void);
        int Update(void);
        void Shutdown(void); 
        static void GlfwErrorCallback(int error, const char *s);
    
        void Instantiate(EngineObject* obj);

        GLFWwindow* main_window;
        int main_view;

        int keyboard_slots[GLFW_KEY_LAST];
        int cursor_slots[GLFW_MOUSE_BUTTON_LAST+1];
        int cursor_xpos;
        int cursor_ypos;
        
        static void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void cursor_callback(GLFWwindow *window, double x, double y);
        static void cursor_button_callback(GLFWwindow *window, int button, int action, int mods);
        static void window_size_callback(GLFWwindow *window, int width, int height);
        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
        static void char_callback(GLFWwindow *window, unsigned int codepoint);

        void reset(void);

    private: 
        int width; 
        int height; 
        std::string title; 

        bgfx::ProgramHandle program;
        bgfx::UniformHandle u_position; 
        bgfx::UniformHandle u_rotation; 
        bgfx::UniformHandle u_scale; 

        std::vector<EngineObject *> objs; 

        float last_time; 
        float dt; 
        float time; 
};

#endif 
