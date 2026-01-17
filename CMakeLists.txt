cmake_minimum_required(VERSION 3.22)
project(learn-opengl VERSION 0.1.0)

include(cmake/prelude.cmake)
include(cmake/create_executable.cmake)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
find_package(stb REQUIRED)
find_package(spdlog REQUIRED)
find_package(assimp REQUIRED)

# include(cmake/fetched-libs.cmake)
include(cmake/imgui.cmake)

add_subdirectory(common)

add_subdirectory(./main/1_getting_started/1.1_hello_window/code)
add_subdirectory(./main/1_getting_started/1.2_hello_triangle/code)
add_subdirectory(./main/1_getting_started/1.3_shaders/code)
add_subdirectory(./main/1_getting_started/1.4_textures/code)
add_subdirectory(./main/1_getting_started/1.5_transformations/code)
add_subdirectory(./main/1_getting_started/1.6_coordinate_systems/code)
add_subdirectory(./main/1_getting_started/1.7_camera/code)

add_subdirectory(./main/2_lighting/2.1_colors/code)
add_subdirectory(./main/2_lighting/2.2_basic_lighting/code)
add_subdirectory(./main/2_lighting/2.3_material/code)
add_subdirectory(./main/2_lighting/2.4_lighting_maps/code)
add_subdirectory(./main/2_lighting/2.5_light_casters/code)
add_subdirectory(./main/2_lighting/2.6_multiple_lights/code)

add_subdirectory(./main/3_model_loading/3.x_using_it/code)

add_subdirectory(./main/4_advanced_opengl/4.01_depth_testing/code)
add_subdirectory(./main/4_advanced_opengl/4.02_stencil_testing/code)
add_subdirectory(./main/4_advanced_opengl/4.03_blending/code)
add_subdirectory(./main/4_advanced_opengl/4.04_face_culling/code)
add_subdirectory(./main/4_advanced_opengl/4.05_framebuffers/code)
add_subdirectory(./main/4_advanced_opengl/4.06_cubemaps/code)
