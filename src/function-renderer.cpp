#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <common.hpp>
#include <engine.hpp>

int main(int argc, char *argv[]) {
    Engine engine;
    
    int ret = engine.Init();
    if (ret < 0) {
        ERROR("failed initializing engine");
        return EXIT_FAILURE;
    }

    while (!glfwWindowShouldClose(engine.main_window)) {
        engine.Update();
    }
    engine.Shutdown();

    return EXIT_SUCCESS;
}
