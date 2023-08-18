cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(glbinding_test)

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED PATHS ./lib/glbinding)

include_directories(
    ./src
    ./include
)

set(MAIN_EXE main)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${MAIN_EXE} ${SOURCES})

target_link_libraries(${MAIN_EXE} PUBLIC
    glfw
    glbinding::glbinding
)

# copy assets to build directory
add_custom_command(
    TARGET ${MAIN_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${MAIN_EXE}>/assets
)
