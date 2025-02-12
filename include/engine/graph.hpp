#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <bgfx/bgfx.h>

class GraphComponent {
    public:
        GraphComponent();

        void Calculate(float x, float y, std::vector<float> &out);

        float (*calculate_callback)(float time, float x, float y); 

        glm::vec3 space_view;
        float sampling_rate; 

        bgfx::VertexLayout graph_layout;
        
        uint64_t render_state; 

        float last_time; 
        float dt; 
        float time; 
};

#endif 
