cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ENABLE_SANTIZER OFF CACHE BOOL "Enable sanitizer")

project(glbinding_test)

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED PATHS ./lib/glbinding)

include_directories(
    ./src
    ./include
)

if (ENABLE_SANTIZER)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)

    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)

    add_compile_options(-fsanitize=leak)
    add_link_options(-fsanitize=leak)
endif()

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
