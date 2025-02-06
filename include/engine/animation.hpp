#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <iostream>
#include <engine.hpp>
#include <engine/object.hpp>

class Animation {
    public:
        Animation(
            Engine *ref,
            std::string slide_texture, 
            int cell_width, 
            int cell_height, 
            int cell_num, 
            float speed
        ); 
        void Cycle(float time, EngineObject* o);
    private:
        Engine *ref;
        bgfx::TextureHandle slide_texture;
        int cell_width;
        int cell_height;
        int cell_num;
        float speed; 
};

#endif
