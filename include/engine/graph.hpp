#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <iostream>
#include <glm/glm.hpp>
#include <bgfx/bgfx.h>

struct polynomial_term {
    float coefficient; 
    int x_degree;
    int y_degree;
};

struct polynomial {
    std::vector<struct polynomial_term> terms;

    float x_offset; 
    float y_offset; 
    float sampling_rate;

    void add_term(float coefficient, int x_degree = 0, int y_degree = 0) {
        struct polynomial_term new_term = {
            .coefficient = coefficient,
            .x_degree = x_degree,
            .y_degree = y_degree
        };
        this->terms.push_back(new_term);
    }

    polynomial() : terms(), x_offset(0.0f), y_offset(0.0f), sampling_rate(0.1f) {

    }
};

class GraphComponent {
    public:
        GraphComponent();

        void LoadPolynomial(struct polynomial p, glm::vec3 space_view = glm::vec3(1.0f));
        void Calculate(float x, float y, std::vector<float> &out);

        struct polynomial p;
        glm::vec3 space_view;

        bgfx::VertexLayout graph_layout;
        
        uint64_t render_state; 
};

#endif 
