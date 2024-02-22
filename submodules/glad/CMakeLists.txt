cmake_minimum_required(VERSION 3.16..3.28)
project(glad
  VERSION 2.0.5
  DESCRIPTION "Multi-Language Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator."
  LANGUAGES C CXX)

file(GLOB_RECURSE GLAD_SRCS ./src/*.c)
file(GLOB_RECURSE GLAD_HDRS ./include/*.h)

add_library(glad ${GLAD_SRCS} ${GLAD_HDRS})
add_library(glad::glad ALIAS glad)
