#ifndef QUAD_HPP
#define QUAD_HPP

#include <iostream>
#include <vector>
#include <bgfx/bgfx.h>

struct VertexData {
    float x; 
    float y; 
    float z; 

    float u; 
    float v;
};

class EngineQuad {
public:
    std::vector<VertexData> vertices;
    std::vector<uint16_t> indices;
    bgfx::VertexLayout layout;
    EngineQuad(void);
    EngineQuad(std::vector<VertexData> _v, std::vector<uint16_t> _i);
};

#endif 
