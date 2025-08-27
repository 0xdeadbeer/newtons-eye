#include "bgfx/defines.h"
#include <engine/board.hpp>

BoardComponent::BoardComponent(glm::vec4 fb, glm::vec4 sb, glm::vec4 tb, glm::vec4 lb)
    : first_body(fb)
    , second_body(sb)
    , third_body(tb)
    , fourth_body(lb)
    , initial_speed(0.0f)
    , masses(
            glm::vec4(1.0f, 5.0f, 0.0f, 0.0f),
            glm::vec4(1.0f, 5.0f, 0.0f, 0.0f),
            glm::vec4(1.0f, 5.0f, 0.0f, 0.0f),
            glm::vec4(1.0f, 5.0f, 0.0f, 0.0f)
            )
    , render_state(BGFX_STATE_WRITE_R |
        BGFX_STATE_WRITE_G | 
        BGFX_STATE_WRITE_B | 
        BGFX_STATE_WRITE_A | 
        BGFX_STATE_WRITE_Z |
        BGFX_STATE_DEPTH_TEST_LESS) {
    this->layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .end();
}

