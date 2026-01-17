# create a new executable
function(create_executable NAME)
  cmake_parse_arguments(
    ARG
    "SANITIZE_MEMORY;SANITIZE_THREAD"
    ""
    "SOURCES;INCLUDE_DIRS;DEPENDS;DEFINES"
    ${ARGN}
  )

  if(ARGS_UNPARSED_ARGUMENTS)
    message(
      FATAL_ERROR
      "cppfront_compile: Unrecognized arguments: ${ARG_UNPARSED_ARGUMENTS}"
    )
  endif()

  add_executable(${NAME} ${ARG_SOURCES})
  target_link_libraries(${NAME} PRIVATE ${ARG_DEPENDS})
  target_compile_definitions(${NAME} PRIVATE ${ARG_DEFINES})
  target_include_directories(${NAME} PRIVATE ${ARG_INCLUDE_DIRS})

  set_target_properties(
    ${NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
  )

  # set additional warnings
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${NAME} PRIVATE /W4)
  else()
    target_compile_options(${NAME} PRIVATE -Wall -Wextra -Wconversion)
  endif()

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(ARG_SANITIZE_MEMORY)
      target_compile_options(${NAME} PRIVATE -fsanitize=leak,address,undefined)
      target_link_options(${NAME} PRIVATE -fsanitize=leak,address,undefined)
    elseif(ARG_SANITIZE_THREAD)
      target_compile_options(${NAME} PRIVATE -fsanitize=thread)
      target_link_options(${NAME} PRIVATE -fsanitize=thread)
    endif()
  elseif(SANITIZE_THREAD OR ARG_SANITIZE_MEMORY)
    message(
      WARNING
      "Sanitizers only available for GCC. Yours: [${CMAKE_CXX_COMPILER_ID}]."
    )
  endif()
endfunction()
