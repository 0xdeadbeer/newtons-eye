#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include <engine/quad.hpp>
#include <glm/glm.hpp>
#include <bgfx/bgfx.h>
#include <assimp/scene.h>

class EngineObject {
    public:
        EngineObject();

        int load_node(aiScene *scene, aiNode *node);
        int load_model(std::string filename);

        glm::vec4 position;
        glm::vec4 rotation;
        glm::vec4 scale;

        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        bgfx::VertexLayout layout;
        bgfx::TextureHandle texture;
        bgfx::VertexBufferHandle vbh;
        bgfx::IndexBufferHandle ibh;
};

#endif
