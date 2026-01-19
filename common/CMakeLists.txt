# new common lib
# --------------
add_library(learnopengl-common STATIC src/util/assets_path.cpp)
target_include_directories(learnopengl-common PUBLIC include)

# old common lib
# --------------
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(glbinding REQUIRED)

add_library(
  learnopengl-common-old
  STATIC
  src/old/window.cpp
  src/old/window_manager.cpp
)
target_include_directories(learnopengl-common-old PUBLIC include)
target_link_libraries(
  learnopengl-common-old
  PUBLIC glfw glm::glm glbinding::glbinding
)

# both common lib
# ---------------
add_library(learnopengl-common-all INTERFACE)
target_link_libraries(
  learnopengl-common-all
  INTERFACE learnopengl-common learnopengl-common-old
)

# aliases
# -------
add_library(learnopengl::common ALIAS learnopengl-common)
add_library(learnopengl::common-old ALIAS learnopengl-common-old)
add_library(learnopengl::common-all ALIAS learnopengl-common-all)
