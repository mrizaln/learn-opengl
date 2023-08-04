cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(ProjectName)

find_package(glfw3 REQUIRED)

add_subdirectory(./lib/glad)
include_directories(./lib/glad/include)

# integrity test
set(MAIN_EXE main)
add_executable(${MAIN_EXE} main.cpp)

target_link_libraries(${MAIN_EXE}
    PUBLIC glad
    PUBLIC glfw
)
