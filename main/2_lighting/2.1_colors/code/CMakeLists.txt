set(NAME 2.1_colors)
set(
  LIBS
  learnopengl::common
  glfw
  glbinding::glbinding
  glm::glm
  stb::stb
)

create_executable(${NAME}
  SOURCES      main.cpp
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
