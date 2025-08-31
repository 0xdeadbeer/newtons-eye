#include "bgfx/defines.h"
#include <engine/board.hpp>

BoardComponent::BoardComponent()
    : positions(
            glm::vec4(-0.5, 0.0f, -0.5f, -0.5f),
            glm::vec4(0.5, 0.0f, 0.5f, -0.5f),
            glm::vec4(0.0, 0.5f, 0.5f, 0.5f),
            glm::vec4(0.0, -0.5f, -0.5f, 0.5f)
        )
    , initial_speed(0.0f)
    , params(1.0f, 1.0f, 250.0f, 0.05f)
    , sparams(0b00000010, 0.0f, 0.0f, 0.0f)
    , masses(
            glm::vec4(1.0f, 5.0f, 1.0f, 5.0f),
            glm::vec4(1.0f, 5.0f, 1.0f, 5.0f),
            glm::vec4(1.0f, 5.0f, 1.0f, 5.0f),
            glm::vec4(1.0f, 5.0f, 1.0f, 5.0f)
            )
    , color_strip_1(
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 1.0f)
        )
    , color_strip_2(
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 0.0f)
        )
    , color_strip_3(
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(0.3f, 0.8f, 0.5f),
            glm::vec3(0.0f)
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

