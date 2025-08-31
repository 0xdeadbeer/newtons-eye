#ifndef BOARD_HPP
#define BOARD_HPP 

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <bgfx/bgfx.h>

class BoardComponent {
    public:
        BoardComponent();

        bgfx::VertexLayout layout;
        uint64_t render_state; 

        glm::vec4 initial_speed; 
        glm::vec4 params; 
        glm::vec4 sparams; 

        glm::mat4 positions; 
        glm::mat4 masses;

        glm::mat3 color_strip_1;
        glm::mat3 color_strip_2;
        glm::mat3 color_strip_3;
};

#endif
