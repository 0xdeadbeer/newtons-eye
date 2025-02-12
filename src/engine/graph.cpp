#include "bgfx/defines.h"
#include <engine/graph.hpp>

GraphComponent::GraphComponent() : space_view(1.0f), sampling_rate(1.0f), render_state(
        BGFX_STATE_WRITE_R |
        BGFX_STATE_WRITE_G | 
        BGFX_STATE_WRITE_B | 
        BGFX_STATE_WRITE_A | 
        BGFX_STATE_WRITE_Z |
        BGFX_STATE_DEPTH_TEST_LESS) {
    this->graph_layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .end();
}

void GraphComponent::Calculate(float x, float y, std::vector<float> &out) {
    out.push_back(x);
    out.push_back(y);
    out.push_back(this->calculate_callback(this->time, x, y));
}
