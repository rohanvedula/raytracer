cmake_minimum_required(VERSION 3.16)
project(CS488)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(external/glew)
add_subdirectory(external/glfw)

file(GLOB SRC_FILES
        src/*.h
        src/*.cpp)

add_executable(CS488
        ${SRC_FILES}
        src/constants.h
        src/image.h
        src/material.h
        src/perlin.h
        src/structures.h
        src/aabb.h
        src/trianglemesh.h
        src/bvh.h
        src/scene.h
        src/randomnum.h
)

target_include_directories(CS488 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include)
target_include_directories(CS488 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/external/glew/include)
target_link_libraries(CS488 glfw libglew_static)
