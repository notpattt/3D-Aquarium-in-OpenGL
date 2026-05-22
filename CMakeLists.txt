cmake_minimum_required(VERSION 3.20)

project(ZadaniaOpenGL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
        "C:/opengl-projects/libraries/include"
        ${CMAKE_CURRENT_SOURCE_DIR}
)

link_directories("C:/opengl-projects/libraries/lib")

add_executable(ZadaniaOpenGL
        main.cpp
        glad.c
        Geometry.cpp
        TextureLoader.cpp
        Fish.cpp
        Scene.cpp
        Chest.cpp
        Aquarium.cpp
        Coconut.cpp
        Table.cpp
)

target_link_libraries(ZadaniaOpenGL glfw3 opengl32)