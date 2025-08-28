#ifndef BOARD_HPP
#define BOARD_HPP 

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <bgfx/bgfx.h>

class BoardComponent {
    public:
        BoardComponent(glm::vec4 fb, glm::vec4 sb, glm::vec4 tb, glm::vec4 lb);

        bgfx::VertexLayout layout;
        uint64_t render_state; 

        glm::vec4 first_body; 
        glm::vec4 second_body; 
        glm::vec4 third_body; 
        glm::vec4 fourth_body; 
        glm::vec4 initial_speed; 
        glm::vec4 params; 

        glm::mat4 masses;
};

#endif
