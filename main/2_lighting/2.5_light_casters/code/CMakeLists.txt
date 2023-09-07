cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# turn off subobject linkage warning (what is this warning about actually?)
add_compile_options(-Wno-subobject-linkage)

project(LearnOpenGL)

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED PATHS ./lib/glbinding)

include_directories(
    ./include
    ./lib/imgui
    ./lib/imgui/backends
)

# imgui stuff
file(GLOB IMGUI_SOURCES "./lib/imgui/*.cpp")
set(IMGUI_SOURCES
    ${IMGUI_SOURCES}
    "./lib/imgui/backends/imgui_impl_glfw.cpp"
    "./lib/imgui/backends/imgui_impl_opengl3.cpp"
)

set(MAIN_EXE main)
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${MAIN_EXE} ${SOURCES} ${IMGUI_SOURCES})

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
