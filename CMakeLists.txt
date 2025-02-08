cmake_minimum_required(VERSION 3.25.1)
project(pgv
        VERSION 0.1
        DESCRIPTION "polynomial graphical viewer"
        HOMEPAGE_URL "http://git.0xdeadbeer.xyz/0xdeadbeer/pgv"
        LANGUAGES CXX)

add_subdirectory("thirdparty/bgfx.cmake")
add_subdirectory("thirdparty/glfw.cmake")
add_subdirectory("thirdparty/assimp.cmake")
add_subdirectory("thirdparty/imgui.cmake")
add_subdirectory("thirdparty/glm.cmake")
add_executable(pgv
    src/pgv.cpp
    src/common.cpp
    src/engine.cpp
    src/engine/quad.cpp
    src/engine/object.cpp
    src/engine/model.cpp
    src/engine/graph.cpp
    src/engine/imgui.cpp
)
target_link_libraries(pgv bx)
target_link_libraries(pgv bgfx)
target_link_libraries(pgv bimg)
target_link_libraries(pgv imgui)
target_link_libraries(pgv glfw)
target_link_libraries(pgv assimp)
target_link_libraries(pgv glm)
target_include_directories(pgv PRIVATE "include/")


# build shaders into /build/shaders/
file(GLOB SHADER_SOURCE_FILES "${PROJECT_SOURCE_DIR}/shaders/*.sc")
set(SHADER_PLATFORM "linux")
set(SHADER_PROFILE "440")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/shaders")
foreach(SHADER_SOURCE ${SHADER_SOURCE_FILES})
    cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
    cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)
    cmake_path(GET SHADER_SOURCE STEM SHADER_STEM)

    set(VERTEX_SHADER_TYPE -1)
    set(FRAGMENT_SHADER_TYPE -1)
    set(SHADER_TYPE)
    string(FIND "${SHADER_NAME}" "vs" VERTEX_SHADER_TYPE)
    string(FIND "${SHADER_NAME}" "fs" FRAGMENT_SHADER_TYPE)
    if(${VERTEX_SHADER_TYPE} GREATER -1) 
        set(SHADER_TYPE "vertex")
    elseif(${FRAGMENT_SHADER_TYPE} GREATER -1)
        set(SHADER_TYPE "fragment")
    else()
        continue()
    endif()

    set(SHADER_COMMAND_ARGUMENTS "-f ${SHADER_SOURCE} -o ${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_LANGUAGE}/${SHADER_STEM}.bin --type ${SHADER_TYPE} --platform ${SHADER_PLATFORM} --profile ${SHADER_PROFILE}")
    set(SHADER_COMMAND "${PROJECT_SOURCE_DIR}/tools/shaderc ${SHADER_COMMAND_ARGUMENTS}") 

    add_custom_target(${SHADER_NAME} ALL
        COMMAND /bin/bash -c "${SHADER_COMMAND}"
    )
endforeach()

# build textures into /build/textures
file(GLOB TEXTURE_SOURCE_FILES "${PROJECT_SOURCE_DIR}/textures/*")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/textures")
foreach(TEXTURE_SOURCE ${TEXTURE_SOURCE_FILES})
    cmake_path(ABSOLUTE_PATH TEXTURE_SOURCE NORMALIZE)
    cmake_path(GET TEXTURE_SOURCE FILENAME TEXTURE_NAME)
    cmake_path(GET TEXTURE_SOURCE STEM TEXTURE_STEM)

    set(TEXTURE_COMMAND_ARGUMENTS "-f ${TEXTURE_SOURCE} -o ${CMAKE_CURRENT_BINARY_DIR}/textures/${TEXTURE_STEM}.dds")
    set(TEXTURE_COMMAND "${PROJECT_SOURCE_DIR}/tools/texturec ${TEXTURE_COMMAND_ARGUMENTS}")

    add_custom_target(${TEXTURE_NAME} ALL 
        COMMAND /bin/bash -c "${TEXTURE_COMMAND}"
    )
endforeach()

# build models into /build/models 
add_custom_target("models" ALL
    COMMAND /bin/bash -c "cp -rf ${PROJECT_SOURCE_DIR}/models/ ${CMAKE_CURRENT_BINARY_DIR}/"
)
# file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/models")
# file(COPY "${PROJECT_SOURCE_DIR}/models/" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/models/")
