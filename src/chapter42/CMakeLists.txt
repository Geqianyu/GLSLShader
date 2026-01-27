file(GLOB_RECURSE CHAPTER_42_FILES
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/gl.c
    ${CMAKE_SOURCE_DIR}/include/common/glsl_program.h
    ${CMAKE_SOURCE_DIR}/src/common/glsl_program.cpp
    ${CMAKE_SOURCE_DIR}/include/common/triangle_mesh.h
    ${CMAKE_SOURCE_DIR}/src/common/triangle_mesh.cpp
    ${CMAKE_SOURCE_DIR}/include/common/cube.h
    ${CMAKE_SOURCE_DIR}/src/common/cube.cpp
    ${CMAKE_SOURCE_DIR}/include/common/sphere.h
    ${CMAKE_SOURCE_DIR}/src/common/sphere.cpp
    ${CMAKE_SOURCE_DIR}/src/chapter42/*.cpp)

add_executable(Chapter42 ${CHAPTER_42_FILES})

target_include_directories(Chapter42 PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(Chapter42 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glfw/include")
target_include_directories(Chapter42 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glad/include")
target_include_directories(Chapter42 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/glm")
target_include_directories(Chapter42 PRIVATE "${CMAKE_SOURCE_DIR}/vendor/stb")

target_link_libraries(Chapter42 glfw)
target_link_libraries(Chapter42 glm)

set_target_properties(Chapter42 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Chapter42")