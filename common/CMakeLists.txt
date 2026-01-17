add_library(learnopengl-common STATIC ./util/assets_path.cpp)
target_include_directories(learnopengl-common PUBLIC .)

add_library(learnopengl::common ALIAS learnopengl-common)
