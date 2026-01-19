set(NAME 1.3_shaders)
set(LIBS learnopengl::common-all glfw glbinding::glbinding)

create_executable(${NAME}
  SOURCES main.cpp
  DEPENDS ${LIBS}
  DEFINES USE_SHADER_HEADER
)

create_executable(${NAME}_no_header
  SOURCES main.cpp
  DEPENDS ${LIBS}
)

add_custom_command(
  TARGET ${NAME}
  POST_BUILD
  COMMENT "copy assets to build directory"
  COMMAND
    ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:${NAME}>/assets/${NAME}
)
