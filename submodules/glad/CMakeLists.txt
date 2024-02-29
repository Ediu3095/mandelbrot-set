cmake_minimum_required(VERSION 3.16..3.28)
project(
  glad
  VERSION 2.0.5
  DESCRIPTION "Multi-Language Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator."
  LANGUAGES C CXX
)

file(GLOB_RECURSE GLAD_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE GLAD_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(${PROJECT_NAME} ${GLAD_SRCS})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(
  ${PROJECT_NAME}
  PUBLIC
    ${GLAD_HDRS}
)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)
