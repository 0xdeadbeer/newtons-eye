#include <engine/animation.hpp>
#include <engine/object.hpp>
#include <common.hpp>

Animation::Animation(
    Engine* ref,
    std::string slide_texture, 
    int cell_width,
    int cell_height, 
    int cell_num, 
    float speed 
) {
    this->ref = ref;
    this->slide_texture = load_texture(slide_texture);
    this->cell_width = cell_width;
    this->cell_height = cell_height;
    this->cell_num = cell_num;
    this->speed = speed;
}

void Animation::Cycle(float time, EngineObject *o) {
    int stage = (int) (time / (this->speed*1000.0f)) % this->cell_num;
    bgfx::blit(
        this->ref->main_view,
        o->texture, 
        0, 
        0, 
        this->slide_texture, 
        stage*cell_width,
        0, 
        this->cell_width,
        this->cell_height
    );
}
