cmake_minimum_required(VERSION 3.22)

project(GLSLShader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter00)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter01)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter02)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter03)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter04)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter05)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter06)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter07)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter08)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter09)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter10)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter11)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/chapter12)

if (MSVC)
    add_compile_options(/utf-8)
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/assets)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()