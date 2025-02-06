#include "bgfx/defines.h"
#include <iostream>
#include <string>
#include <bx/filepath.h>
#include <bx/file.h>
#include <bx/string.h>
#include <bgfx/bgfx.h>
#include <common.hpp>

bx::DefaultAllocator default_allocator;
bx::FileReader* reader = BX_NEW(&default_allocator, bx::FileReader);

bgfx::Memory* load_mem(bx::FileReader* reader, bx::FilePath& filepath) {
	if (!bx::open(reader, filepath)) {
        ERROR("--error: failed to load " + std::string(filepath.getCPtr()));
        return NULL;
	}

    uint32_t size = (uint32_t)bx::getSize(reader);
    const bgfx::Memory* mem = bgfx::alloc(size+1);

    bx::read(reader, mem->data, size, bx::ErrorAssert{});
    bx::close(reader);

    mem->data[mem->size-1] = '\0';

    return (bgfx::Memory *) mem;
}

bgfx::ShaderHandle load_shader(bx::FileReader* reader, std::string filename) {
    bx::FilePath filepath(filename.c_str());
    bgfx::ShaderHandle handle = bgfx::createShader(
            load_mem(reader, filepath));
    
    LOG("loading shader: " + filename);

    bgfx::setName(handle, filename.c_str(), filename.length());
    return handle;
}

bgfx::ProgramHandle load_program(std::string vs_name,
        std::string fs_name) {
    bgfx::ShaderHandle vsh = load_shader(reader, vs_name);
    bgfx::ShaderHandle fsh = load_shader(reader, fs_name);

    return bgfx::createProgram(vsh, fsh, true);
}

bgfx::TextureHandle load_texture(std::string filename) {
    bx::FilePath filepath(filename.c_str());
    return bgfx::createTexture(load_mem(reader, filepath), BGFX_TEXTURE_BLIT_DST|BGFX_SAMPLER_POINT);
}
