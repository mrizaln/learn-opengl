if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(imgui_PACKAGE_DIR "${imgui_PACKAGE_FOLDER_DEBUG}")
else()
  set(imgui_PACKAGE_DIR "${imgui_PACKAGE_FOLDER_RELEASE}")
endif()

set(IMGUI_BINDINGS_INCLUDE_DIR "${imgui_PACKAGE_DIR}/res/bindings")
set(
  IMGUI_BINDINGS
  "${IMGUI_BINDINGS_INCLUDE_DIR}/imgui_impl_glfw.cpp"
  "${IMGUI_BINDINGS_INCLUDE_DIR}/imgui_impl_opengl3.cpp"
)
message(${IMGUI_BINDINGS_INCLUDE_DIR})

add_library(imgui_glfw OBJECT ${IMGUI_BINDINGS})
target_include_directories(imgui_glfw PUBLIC ${IMGUI_BINDINGS_INCLUDE_DIR})
target_link_libraries(imgui_glfw PRIVATE imgui::imgui glfw)
add_library(imgui::glfw ALIAS imgui_glfw)
