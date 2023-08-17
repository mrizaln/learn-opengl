cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ENABLE_SANTIZER OFF CACHE BOOL "Enable sanitizer")

add_compile_options(-Wno-subobject-linkage)    # turn off subobject linkage warning

project(multi_context_opengl)

find_package(glfw3 REQUIRED)

include_directories(
    ${glfw3_INCLUDE_DIRS}
    ./src
    ./include
)

add_subdirectory(./lib/glbinding)

if (ENABLE_SANTIZER)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)

    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)

    add_compile_options(-fsanitize=leak)
    add_link_options(-fsanitize=leak)
endif()

file(GLOB_RECURSE SRC_FILES src/*.cpp)
set(MAIN_EXE main)
add_executable(${MAIN_EXE} ${SRC_FILES})

target_include_directories(${MAIN_EXE} PUBLIC
    ./include
    ./src
)

target_link_libraries(${MAIN_EXE} PUBLIC
    glfw
    glbinding::glbinding
    glbinding::glbinding-aux
)

add_custom_command(
    TARGET ${MAIN_EXE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${MAIN_EXE}>/assets
)
