cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ENABLE_SANTIZER OFF CACHE BOOL "Enable sanitizer")

project(LearnOpenGL)

find_package(glfw3 REQUIRED)
find_package(glbinding REQUIRED)
find_package(glm REQUIRED)
find_package(stb REQUIRED)
find_package(imgui REQUIRED)

# get the imgui glfw bindings
# ---------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(imgui_PACKAGE_DIR "${imgui_PACKAGE_FOLDER_DEBUG}")
else()
    set(imgui_PACKAGE_DIR "${imgui_PACKAGE_FOLDER_RELEASE}")
endif()
set(IMGUI_BINDINGS_INCLUDE_DIR "${imgui_PACKAGE_DIR}/res/bindings")
set(IMGUI_BINDINGS
    "${IMGUI_BINDINGS_INCLUDE_DIR}/imgui_impl_glfw.cpp"
    "${IMGUI_BINDINGS_INCLUDE_DIR}/imgui_impl_opengl3.cpp"
)
message(${IMGUI_BINDINGS_INCLUDE_DIR})

add_library(imgui_backend_glfw OBJECT ${IMGUI_BINDINGS})
target_include_directories(imgui_backend_glfw PUBLIC ${IMGUI_BINDINGS_INCLUDE_DIR})
target_link_libraries(imgui_backend_glfw PRIVATE imgui::imgui)
# ---------------------------

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
target_include_directories(${MAIN_EXE} PRIVATE ./include)
target_link_libraries(${MAIN_EXE} PRIVATE glfw glbinding::glbinding glm::glm stb::stb imgui::imgui imgui_backend_glfw)

# copy assets to build directory
add_custom_command(
    TARGET ${MAIN_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${MAIN_EXE}>/assets
)
