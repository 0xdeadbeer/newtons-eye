#include <engine/quad.hpp>
#include <bgfx/bgfx.h>

EngineQuad::EngineQuad(void) {
    this->vertices = {
        {-0.5f, -0.5f, 0.0f, 0, 1},
        {-0.5f, 0.5f, 0.0f, 0, 0},
        {0.5f, -0.5f, 0.0f, 1, 1},
        {0.5f, 0.5f, 0.0f, 1, 0}
    };
    this->indices = {0, 1, 2, 1, 2, 3};
    this->layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
}

EngineQuad::EngineQuad(std::vector<VertexData> _v, std::vector<uint16_t> _i) {
    this->vertices = _v;
    this->indices = _i;
    this->layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
}

