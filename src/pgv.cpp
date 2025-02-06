#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <common.hpp>
#include <engine.hpp>
#include <engine/animation.hpp>
#include <chrono>

#define GET_MS \
    std::chrono::duration_cast<std::chrono::milliseconds>( \
        std::chrono::system_clock::now().time_since_epoch() \
    ); 

int main(int argc, char *argv[]) {
    Engine engine;
    
    int ret = engine.Init();
    std::cout << "ret " << ret << std::endl;
    if (ret < 0) {
        ERROR("failed initializing engine");
        return EXIT_FAILURE;
    }

    EngineObject obj;
    obj.load_model("models/cube.obj");
    obj.position.z = -30.0f;

    engine.Instantiate(&obj);
    while (!glfwWindowShouldClose(engine.main_window)) {
        engine.Update();
    }
    engine.Shutdown();

    return EXIT_SUCCESS;
}
