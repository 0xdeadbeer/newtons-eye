#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <bgfx/bgfx.h>
#include <bx/file.h>

#define GLFW_DEBUG

#define DEFAULT_WIDTH 700
#define DEFAULT_HEIGHT 700

#define LOG(x) std::cout << "--log: " << x << std::endl; 
#define ERROR(x) std::cout << "--error: " << x << std::endl; 

#define DEFAULT_VERTEX "shaders/vs_basic.bin"
#define DEFAULT_FRAGMENT "shaders/fs_basic.bin"

bgfx::Memory* load_mem(bx::FileReader* reader, bx::FilePath& filepath); 
bgfx::ShaderHandle load_shader(bx::FileReader* reader, std::string filename); 
bgfx::ProgramHandle load_program(std::string vs_name, std::string fs_name);
bgfx::TextureHandle load_texture(std::string filename);

#endif
