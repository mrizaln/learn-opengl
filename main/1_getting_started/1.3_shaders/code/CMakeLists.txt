cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(LearnOpenGL)

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)

set(MAIN_EXE main)
add_executable(${MAIN_EXE} main.cpp)
target_link_libraries(${MAIN_EXE} PUBLIC glfw glbinding::glbinding)

# copy assets to build directory
add_custom_command(
    TARGET ${MAIN_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${MAIN_EXE}>/assets
)
