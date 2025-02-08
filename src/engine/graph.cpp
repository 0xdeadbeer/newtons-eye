#include <engine/graph.hpp>

GraphComponent::GraphComponent() : p{}, space_view(1.0f), render_state(
        BGFX_STATE_WRITE_R |
        BGFX_STATE_WRITE_G | 
        BGFX_STATE_WRITE_B | 
        BGFX_STATE_WRITE_A | 
        BGFX_STATE_WRITE_Z |
        BGFX_STATE_PT_LINESTRIP) {
    this->graph_layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .end();
}

void GraphComponent::LoadPolynomial(struct polynomial p, glm::vec3 space_view) {
    this->p = p;
    this->space_view = space_view;
}

void GraphComponent::Calculate(float x, std::vector<float> &out) {
    struct polynomial *p = &this->p;
    float y = p->y_offset;
    for (int term = 0; term < p->terms.size(); ++term) {
        struct polynomial_term *t = &p->terms.at(term);

        y += t->coefficient * std::pow(x+p->x_offset, t->x_degree); 
    }

    out.push_back(x);
    out.push_back(y);
    out.push_back(0.0f);
}
