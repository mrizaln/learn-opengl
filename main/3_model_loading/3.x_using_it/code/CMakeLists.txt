set(NAME 3_model_loading)
set(
  LIBS
  learnopengl::common-all
  glfw
  glbinding::glbinding
  glm::glm
  stb::stb
  imgui::imgui
  imgui::glfw
  assimp::assimp
)

create_executable(${NAME}
  SOURCES      src/main.cpp
  INCLUDE_DIRS include
  DEPENDS      ${LIBS}
)

add_custom_command(
  TARGET ${NAME}
  POST_BUILD
  COMMENT "copy assets file to build directory"
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${NAME}>/assets/${NAME}
)
