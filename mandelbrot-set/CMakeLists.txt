file(GLOB_RECURSE SOURCES *.cc)
file(GLOB_RECURSE HEADERS *.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
    glfw
    glm::glm
    glad::glad
)
