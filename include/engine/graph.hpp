#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <glm/glm.hpp>
#include <bgfx/bgfx.h>

struct polynomial_term {
    float coefficient; 
    int x_degree;
};

struct polynomial {
    std::vector<struct polynomial_term> terms;
    void add_term(float coefficient, int x_degree) {
        struct polynomial_term new_term = {
            .coefficient = coefficient,
            .x_degree = x_degree,
        };
        this->terms.push_back(new_term);
    }
};

class GraphComponent {
    public:
        GraphComponent();

        void LoadPolynomial(struct polynomial p, glm::vec3 space_view = glm::vec3(1.0f));

        struct polynomial p;
        glm::vec3 space_view;

        bgfx::VertexLayout graph_layout;
        
        uint64_t render_state; 
};

#endif 
