#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <iostream>
#include <bx/file.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <engine/object.hpp>
#include <memory>

#define CAMERA_WIDTH 50.0f
#define CAMERA_NEAR 0.01f
#define CAMERA_FAR 100.0f

#define INPUT_A         0b00000001
#define INPUT_D         0b00000010
#define INPUT_W         0b00000100
#define INPUT_S         0b00001000
#define INPUT_ZOOM_OUT  0b00010000
#define INPUT_ZOOM_IN   0b00100000

class Engine {
    public:
        Engine(void);
        int Init(void);
        void Update(void);
        void Shutdown(void); 
        int UserLoad(void);
        void UserUpdate(void);
        void ImguiUpdate(EngineObject *obj);
        void Reset(void);
    
        GLFWwindow* main_window;
        int main_view;

        int keyboard_slots[GLFW_KEY_LAST];
        int cursor_slots[GLFW_MOUSE_BUTTON_LAST+1];
        int cursor_xpos;
        int cursor_ypos;

        void CalculateGraph(float start, 
                float end, 
                float sampling_rate, 
                float freq, 
                float amp, 
                float x_offset, 
                float y_offset);
            
        static void error_callback(int error, const char *s);
        static void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void cursor_callback(GLFWwindow *window, double x, double y);
        static void cursor_button_callback(GLFWwindow *window, int button, int action, int mods);
        static void window_size_callback(GLFWwindow *window, int width, int height);
        static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
        static void char_callback(GLFWwindow *window, unsigned int codepoint);

        unsigned char input_map;

    private: 
        int width; 
        int height; 
        std::string title; 

        bgfx::ProgramHandle program;
        bgfx::UniformHandle u_position; 
        bgfx::UniformHandle u_rotation; 
        bgfx::UniformHandle u_scale; 

        std::vector<EngineObject> objs; 

        float last_time; 
        float dt; 
        float time; 

        int debug_flag;
};

#endif 
