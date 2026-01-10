cmake_minimum_required(VERSION 3.22)

project(GLSLShader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter0)

if (MSVC)
    add_compile_options(/utf-8)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()